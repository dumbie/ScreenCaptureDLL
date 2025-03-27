#pragma once
#include "CaptureVariables.h"
#include "CaptureReset.cpp"
#include "CaptureScreen.cpp"
#include "PlayAudio.cpp"

namespace
{
	CaptureResult BitmapDataSaveFile(BYTE* bitmapData, const WCHAR* filePath, GUID iWicFormatGuid, UINT imageQuality)
	{
		AVFinally(
			{
				BitmapImageResetVariablesAll();
			});
		try
		{
			//Create wic factory
			hResult = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_ALL, IID_IWICImagingFactory, (LPVOID*)&vBitmapImageInstance.iWICImagingFactory);
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"Failed creating wic factory") };
			}

			//Create wic stream
			hResult = vBitmapImageInstance.iWICImagingFactory->CreateStream(&vBitmapImageInstance.iWICStream);
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"Failed creating wic stream") };
			}

			//Initialize wic file
			hResult = vBitmapImageInstance.iWICStream->InitializeFromFilename(filePath, GENERIC_WRITE);
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"Failed initializing wic file") };
			}

			//Create bitmap encoder
			hResult = vBitmapImageInstance.iWICImagingFactory->CreateEncoder(iWicFormatGuid, NULL, &vBitmapImageInstance.iWICBitmapEncoder);
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"Failed creating bitmap encoder") };
			}

			//Initialize bitmap encoder
			hResult = vBitmapImageInstance.iWICBitmapEncoder->Initialize(vBitmapImageInstance.iWICStream, WICBitmapEncoderNoCache);
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"Failed initializing bitmap encoder") };
			}

			//Create bitmap frame
			hResult = vBitmapImageInstance.iWICBitmapEncoder->CreateNewFrame(&vBitmapImageInstance.iWICBitmapFrameEncode, &vBitmapImageInstance.iPropertyBag2);
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"Failed creating bitmap frame") };
			}

			//Write property bag
			if (iWicFormatGuid == GUID_ContainerFormatJpeg || iWicFormatGuid == GUID_ContainerFormatWmp || iWicFormatGuid == GUID_ContainerFormatHeif)
			{
				PROPBAG2 propertyValue{};
				propertyValue.pstrName = const_cast<WCHAR*>(L"ImageQuality");

				VARIANT variantValue{};
				variantValue.vt = VT_R4;
				variantValue.fltVal = imageQuality / 100.0F;

				vBitmapImageInstance.iPropertyBag2->Write(1, &propertyValue, &variantValue);
			}

			//Initialize bitmap frame
			hResult = vBitmapImageInstance.iWICBitmapFrameEncode->Initialize(vBitmapImageInstance.iPropertyBag2);
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"Failed initializing bitmap frame") };
			}

			//Bitmap frame set metadata
			hResult = vBitmapImageInstance.iWICBitmapFrameEncode->GetMetadataQueryWriter(&vBitmapImageInstance.iWICMetadataQueryWriter);
			if (SUCCEEDED(hResult))
			{
				//Set application name
				PROPVARIANT propVariantAppName{};
				propVariantAppName.vt = VT_LPSTR;
				propVariantAppName.pszVal = const_cast<CHAR*>("ScreenCaptureDLL");
				if (iWicFormatGuid == GUID_ContainerFormatPng)
				{
					vBitmapImageInstance.iWICMetadataQueryWriter->SetMetadataByName(L"/tEXt/{str=Software}", &propVariantAppName);
				}
				else
				{
					vBitmapImageInstance.iWICMetadataQueryWriter->SetMetadataByName(L"System.ApplicationName", &propVariantAppName);
				}

				//Set date taken
				FILETIME currentFileTime;
				SYSTEMTIME currentSystemTime;
				GetSystemTime(&currentSystemTime);
				SystemTimeToFileTime(&currentSystemTime, &currentFileTime);

				PROPVARIANT propVariantDateTaken{};
				propVariantDateTaken.vt = VT_FILETIME;
				propVariantDateTaken.filetime = currentFileTime;
				vBitmapImageInstance.iWICMetadataQueryWriter->SetMetadataByName(L"System.Photo.DateTaken", &propVariantDateTaken);
			}

			//Bitmap frame set size
			hResult = vBitmapImageInstance.iWICBitmapFrameEncode->SetSize(vCaptureDetails.OutputWidth, vCaptureDetails.OutputHeight);
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"Failed setting bitmap frame size") };
			}

			AVDebugWriteLine("Bitmap frame size, Width: " << vCaptureDetails.OutputWidth << " Height: " << vCaptureDetails.OutputHeight);

			//Bitmap frame set resolution
			hResult = vBitmapImageInstance.iWICBitmapFrameEncode->SetResolution(96, 96);
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"Failed setting bitmap frame resolution") };
			}

			//Write data to bitmap frame and convert jpg
			if (iWicFormatGuid == GUID_ContainerFormatJpeg && vBitmapImageInstance.iWicPixelFormatGuidSource != vBitmapImageInstance.iWicPixelFormatGuidJpeg)
			{
				//Create bitmap from memory
				hResult = vBitmapImageInstance.iWICImagingFactory->CreateBitmapFromMemory(vCaptureDetails.OutputWidth, vCaptureDetails.OutputHeight, vBitmapImageInstance.iWicPixelFormatGuidSource, vCaptureDetails.WidthByteSize, vCaptureDetails.TotalByteSize, bitmapData, &vBitmapImageInstance.iWICBitmap);
				if (FAILED(hResult))
				{
					return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"Failed creating bitmap from memory") };
				}

				//Create wic format converter
				hResult = vBitmapImageInstance.iWICImagingFactory->CreateFormatConverter(&vBitmapImageInstance.iWICFormatConverter);
				if (FAILED(hResult))
				{
					return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"Failed creating wic format converter") };
				}

				//Initialize wic format converter
				hResult = vBitmapImageInstance.iWICFormatConverter->Initialize(vBitmapImageInstance.iWICBitmap, vBitmapImageInstance.iWicPixelFormatGuidJpeg, WICBitmapDitherTypeNone, 0, 0, WICBitmapPaletteTypeCustom);
				if (FAILED(hResult))
				{
					return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"Failed initializing wic format converter") };
				}

				//Bitmap frame set pixel format
				hResult = vBitmapImageInstance.iWICBitmapFrameEncode->SetPixelFormat(&vBitmapImageInstance.iWicPixelFormatGuidJpeg);
				if (FAILED(hResult))
				{
					return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"Failed setting bitmap frame pixel format") };
				}

				//Bitmap encode write
				WICRect iWicRectangle = { 0, 0, (INT)vCaptureDetails.OutputWidth, (INT)vCaptureDetails.OutputHeight };
				hResult = vBitmapImageInstance.iWICBitmapFrameEncode->WriteSource(vBitmapImageInstance.iWICFormatConverter, &iWicRectangle);
				if (FAILED(hResult))
				{
					return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"Failed encoding bitmap write") };
				}
			}
			else
			{
				//Bitmap frame set pixel format
				hResult = vBitmapImageInstance.iWICBitmapFrameEncode->SetPixelFormat(&vBitmapImageInstance.iWicPixelFormatGuidSource);
				if (FAILED(hResult))
				{
					return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"Failed setting bitmap frame pixel format") };
				}

				//Bitmap frame encode write
				hResult = vBitmapImageInstance.iWICBitmapFrameEncode->WritePixels(vCaptureDetails.OutputHeight, vCaptureDetails.WidthByteSize, vCaptureDetails.TotalByteSize, bitmapData);
				if (FAILED(hResult))
				{
					return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"Failed encoding bitmap frame write") };
				}
			}

			//Bitmap frame encode commit
			hResult = vBitmapImageInstance.iWICBitmapFrameEncode->Commit();
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"Failed encoding bitmap frame commit") };
			}

			//Bitmap encode commit
			hResult = vBitmapImageInstance.iWICBitmapEncoder->Commit();
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"Failed encoding bitmap commit") };
			}

			//Return result
			return { .Status = CaptureStatus::Success };
		}
		catch (...)
		{
			//Return result
			return { .Status = CaptureStatus::Failed, .Message = SysAllocString(L"BitmapDataSaveFile failed") };
		}
	}

	CaptureResult CaptureImageCode(const WCHAR* filePath, UINT imageQuality, ImageFormats imageFormat)
	{
		try
		{
			//Check image save format
			GUID imageSaveFormat{};
			if (imageFormat == JXR)
			{
				imageSaveFormat = GUID_ContainerFormatWmp;
			}
			else if (imageFormat == JPG)
			{
				if (vCaptureDetails.HDREnabled && !vCaptureDetails.HDRtoSDR)
				{
					//Play audio effect
					PlayAudio(L"Assets\\Capture\\CaptureFailed.mp3");

					//Return result
					return { .Status = CaptureStatus::Failed, .Message = SysAllocString(L"Image format does not support HDR") };
				}
				imageSaveFormat = GUID_ContainerFormatJpeg;
			}
			else if (imageFormat == PNG)
			{
				if (vCaptureDetails.HDREnabled && !vCaptureDetails.HDRtoSDR)
				{
					//Play audio effect
					PlayAudio(L"Assets\\Capture\\CaptureFailed.mp3");

					//Return result
					return { .Status = CaptureStatus::Failed, .Message = SysAllocString(L"Image format does not support HDR") };
				}
				imageSaveFormat = GUID_ContainerFormatPng;
			}
			else if (imageFormat == BMP)
			{
				if (vCaptureDetails.HDREnabled && !vCaptureDetails.HDRtoSDR)
				{
					//Play audio effect
					PlayAudio(L"Assets\\Capture\\CaptureFailed.mp3");

					//Return result
					return { .Status = CaptureStatus::Failed, .Message = SysAllocString(L"Image format does not support HDR") };
				}
				imageSaveFormat = GUID_ContainerFormatBmp;
			}
			else if (imageFormat == TIF)
			{
				if (vCaptureDetails.HDREnabled && !vCaptureDetails.HDRtoSDR)
				{
					//Play audio effect
					PlayAudio(L"Assets\\Capture\\CaptureFailed.mp3");

					//Return result
					return { .Status = CaptureStatus::Failed, .Message = SysAllocString(L"Image format does not support HDR") };
				}
				imageSaveFormat = GUID_ContainerFormatTiff;
			}
			else if (imageFormat == HEIF)
			{
				if (vCaptureDetails.HDREnabled && !vCaptureDetails.HDRtoSDR)
				{
					//Play audio effect
					PlayAudio(L"Assets\\Capture\\CaptureFailed.mp3");

					//Return result
					return { .Status = CaptureStatus::Failed, .Message = SysAllocString(L"Image format does not support HDR") };
				}
				imageSaveFormat = GUID_ContainerFormatHeif;
			}

			//Get screen bytes
			std::vector<BYTE> screenBytes = GetScreenBytes(false);

			//Check screen bytes
			if (screenBytes.empty())
			{
				//Play audio effect
				PlayAudio(L"Assets\\Capture\\CaptureFailed.mp3");

				//Return result
				return { .Status = CaptureStatus::Failed, .Message = SysAllocString(L"Image bytes are empty") };
			}

			//Save bitmap data to file
			CaptureResult captureResult = BitmapDataSaveFile(screenBytes.data(), filePath, imageSaveFormat, imageQuality);

			//Play audio effect
			if (captureResult.Status == CaptureStatus::Success)
			{
				PlayAudio(L"Assets\\Capture\\CaptureScreenshot.mp3");
			}
			else
			{
				PlayAudio(L"Assets\\Capture\\CaptureFailed.mp3");
			}

			//Return result
			return captureResult;
		}
		catch (...)
		{
			//Play audio effect
			PlayAudio(L"Assets\\Capture\\CaptureFailed.mp3");

			//Return result
			return { .Status = CaptureStatus::Failed, .Message = SysAllocString(L"CaptureImageCode failed") };
		}
	}
}