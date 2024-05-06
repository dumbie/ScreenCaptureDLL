#pragma once
#include "CaptureVariables.h"
#include "CaptureReset.cpp"
#include "CaptureScreen.cpp"
#include "PlayAudio.cpp"

namespace
{
	BOOL BitmapDataSaveFile(BYTE* bitmapData, WCHAR* filePath, GUID iWicFormatGuid, UINT imageQuality)
	{
		try
		{
			//Create wic factory
			hResult = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_ALL, IID_IWICImagingFactory, (LPVOID*)&vBitmapImageInstance.iWICImagingFactory);
			if (FAILED(hResult))
			{
				BitmapImageResetVariablesAll();
				return false;
			}

			hResult = vBitmapImageInstance.iWICImagingFactory->CreateStream(&vBitmapImageInstance.iWICStream);
			if (FAILED(hResult))
			{
				BitmapImageResetVariablesAll();
				return false;
			}

			hResult = vBitmapImageInstance.iWICStream->InitializeFromFilename(filePath, GENERIC_WRITE);
			if (FAILED(hResult))
			{
				BitmapImageResetVariablesAll();
				return false;
			}

			//Create bitmap encoder
			hResult = vBitmapImageInstance.iWICImagingFactory->CreateEncoder(iWicFormatGuid, NULL, &vBitmapImageInstance.iWICBitmapEncoder);
			if (FAILED(hResult))
			{
				BitmapImageResetVariablesAll();
				return false;
			}

			hResult = vBitmapImageInstance.iWICBitmapEncoder->Initialize(vBitmapImageInstance.iWICStream, WICBitmapEncoderNoCache);
			if (FAILED(hResult))
			{
				BitmapImageResetVariablesAll();
				return false;
			}

			//Create bitmap frame
			hResult = vBitmapImageInstance.iWICBitmapEncoder->CreateNewFrame(&vBitmapImageInstance.iWICBitmapFrameEncode, &vBitmapImageInstance.iPropertyBag2);
			if (FAILED(hResult))
			{
				BitmapImageResetVariablesAll();
				return false;
			}

			//Write property bag
			if (iWicFormatGuid == GUID_ContainerFormatJpeg || iWicFormatGuid == GUID_ContainerFormatWmp || iWicFormatGuid == GUID_ContainerFormatHeif)
			{
				PROPBAG2 propertyValue{};
				propertyValue.pstrName = L"ImageQuality";

				VARIANT variantValue{};
				variantValue.vt = VT_R4;
				variantValue.fltVal = imageQuality / 100.0F;

				vBitmapImageInstance.iPropertyBag2->Write(1, &propertyValue, &variantValue);
			}

			//Initialize bitmap frame
			hResult = vBitmapImageInstance.iWICBitmapFrameEncode->Initialize(vBitmapImageInstance.iPropertyBag2);
			if (FAILED(hResult))
			{
				BitmapImageResetVariablesAll();
				return false;
			}

			//Bitmap frame set metadata
			if (SUCCEEDED(vBitmapImageInstance.iWICBitmapFrameEncode->GetMetadataQueryWriter(&vBitmapImageInstance.iWICMetadataQueryWriter)))
			{
				//Set application name
				PROPVARIANT propVariantAppName{};
				propVariantAppName.vt = VT_LPSTR;
				propVariantAppName.pszVal = "ScreenCaptureDLL";
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
				BitmapImageResetVariablesAll();
				return false;
			}

			AVDebugWriteLine("Bitmap frame size, Width: " << vCaptureDetails.OutputWidth << " Height: " << vCaptureDetails.OutputHeight);

			//Bitmap frame set resolution
			hResult = vBitmapImageInstance.iWICBitmapFrameEncode->SetResolution(96, 96);
			if (FAILED(hResult))
			{
				BitmapImageResetVariablesAll();
				return false;
			}

			//Write data to bitmap frame and convert jpg
			if (iWicFormatGuid == GUID_ContainerFormatJpeg && vBitmapImageInstance.iWicPixelFormatGuidSource != vBitmapImageInstance.iWicPixelFormatGuidJpeg)
			{
				hResult = vBitmapImageInstance.iWICImagingFactory->CreateBitmapFromMemory(vCaptureDetails.OutputWidth, vCaptureDetails.OutputHeight, vBitmapImageInstance.iWicPixelFormatGuidSource, vCaptureDetails.WidthByteSize, vCaptureDetails.TotalByteSize, bitmapData, &vBitmapImageInstance.iWICBitmap);
				if (FAILED(hResult))
				{
					BitmapImageResetVariablesAll();
					return false;
				}

				hResult = vBitmapImageInstance.iWICImagingFactory->CreateFormatConverter(&vBitmapImageInstance.iWICFormatConverter);
				if (FAILED(hResult))
				{
					BitmapImageResetVariablesAll();
					return false;
				}

				hResult = vBitmapImageInstance.iWICFormatConverter->Initialize(vBitmapImageInstance.iWICBitmap, vBitmapImageInstance.iWicPixelFormatGuidJpeg, WICBitmapDitherTypeNone, 0, 0, WICBitmapPaletteTypeCustom);
				if (FAILED(hResult))
				{
					BitmapImageResetVariablesAll();
					return false;
				}

				//Bitmap frame set pixelformat
				hResult = vBitmapImageInstance.iWICBitmapFrameEncode->SetPixelFormat(&vBitmapImageInstance.iWicPixelFormatGuidJpeg);
				if (FAILED(hResult))
				{
					BitmapImageResetVariablesAll();
					return false;
				}

				WICRect iWicRectangle = { 0, 0, (INT)vCaptureDetails.OutputWidth, (INT)vCaptureDetails.OutputHeight };
				hResult = vBitmapImageInstance.iWICBitmapFrameEncode->WriteSource(vBitmapImageInstance.iWICFormatConverter, &iWicRectangle);
				if (FAILED(hResult))
				{
					BitmapImageResetVariablesAll();
					return false;
				}
			}
			else
			{
				//Bitmap frame set pixelformat
				hResult = vBitmapImageInstance.iWICBitmapFrameEncode->SetPixelFormat(&vBitmapImageInstance.iWicPixelFormatGuidSource);
				if (FAILED(hResult))
				{
					BitmapImageResetVariablesAll();
					return false;
				}

				hResult = vBitmapImageInstance.iWICBitmapFrameEncode->WritePixels(vCaptureDetails.OutputHeight, vCaptureDetails.WidthByteSize, vCaptureDetails.TotalByteSize, bitmapData);
				if (FAILED(hResult))
				{
					BitmapImageResetVariablesAll();
					return false;
				}
			}

			//Commit bitmap frame
			hResult = vBitmapImageInstance.iWICBitmapFrameEncode->Commit();
			if (FAILED(hResult))
			{
				BitmapImageResetVariablesAll();
				return false;
			}

			//Commit bitmap encoder
			hResult = vBitmapImageInstance.iWICBitmapEncoder->Commit();
			if (FAILED(hResult))
			{
				BitmapImageResetVariablesAll();
				return false;
			}

			//Release resources
			BitmapImageResetVariablesAll();

			return true;
		}
		catch (...)
		{
			BitmapImageResetVariablesAll();
			return false;
		}
	}

	BOOL CaptureImageCode(WCHAR* filePath, UINT imageQuality, ImageFormats imageFormat)
	{
		try
		{
			//Get screen bytes
			std::vector<BYTE> screenBytes = GetScreenBytes(false);

			//Check screen bytes
			if (screenBytes.empty())
			{
				//Play audio effect
				PlayAudio(L"Assets\\Capture\\CaptureFailed.mp3");

				AVDebugWriteLine("Screen capture image bytes are empty.");
				return false;
			}

			//Check image save format
			GUID imageSaveFormat{};
			if (imageFormat == JXR)
			{
				imageSaveFormat = GUID_ContainerFormatWmp;
			}
			else if (imageFormat == JPG)
			{
				if (vCaptureDetails.HDREnabled && !vCaptureDetails.HDRtoSDR) { return false; }
				imageSaveFormat = GUID_ContainerFormatJpeg;
			}
			else if (imageFormat == PNG)
			{
				if (vCaptureDetails.HDREnabled && !vCaptureDetails.HDRtoSDR) { return false; }
				imageSaveFormat = GUID_ContainerFormatPng;
			}
			else if (imageFormat == BMP)
			{
				if (vCaptureDetails.HDREnabled && !vCaptureDetails.HDRtoSDR) { return false; }
				imageSaveFormat = GUID_ContainerFormatBmp;
			}
			else if (imageFormat == TIF)
			{
				if (vCaptureDetails.HDREnabled && !vCaptureDetails.HDRtoSDR) { return false; }
				imageSaveFormat = GUID_ContainerFormatTiff;
			}
			else if (imageFormat == HEIF)
			{
				if (vCaptureDetails.HDREnabled && !vCaptureDetails.HDRtoSDR) { return false; }
				imageSaveFormat = GUID_ContainerFormatHeif;
			}

			//Save bitmap data to file
			BOOL savedBitmap = BitmapDataSaveFile(screenBytes.data(), filePath, imageSaveFormat, imageQuality);

			//Play audio effect
			if (savedBitmap)
			{
				PlayAudio(L"Assets\\Capture\\CaptureScreenshot.mp3");
			}
			else
			{
				PlayAudio(L"Assets\\Capture\\CaptureFailed.mp3");
			}

			//Return result
			return savedBitmap;
		}
		catch (...)
		{
			return false;
		}
	}
}