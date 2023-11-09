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
			hResult = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_ALL, IID_IWICImagingFactory, (LPVOID*)&vCaptureInstance.iWICImagingFactory);
			if (FAILED(hResult))
			{
				CaptureResetVariablesBitmapImage();
				return false;
			}

			hResult = vCaptureInstance.iWICImagingFactory->CreateStream(&vCaptureInstance.iWICStream);
			if (FAILED(hResult))
			{
				CaptureResetVariablesBitmapImage();
				return false;
			}

			hResult = vCaptureInstance.iWICStream->InitializeFromFilename(filePath, GENERIC_WRITE);
			if (FAILED(hResult))
			{
				CaptureResetVariablesBitmapImage();
				return false;
			}

			//Create bitmap encoder
			hResult = vCaptureInstance.iWICImagingFactory->CreateEncoder(iWicFormatGuid, NULL, &vCaptureInstance.iWICBitmapEncoder);
			if (FAILED(hResult))
			{
				CaptureResetVariablesBitmapImage();
				return false;
			}

			hResult = vCaptureInstance.iWICBitmapEncoder->Initialize(vCaptureInstance.iWICStream, WICBitmapEncoderNoCache);
			if (FAILED(hResult))
			{
				CaptureResetVariablesBitmapImage();
				return false;
			}

			//Create bitmap frame
			hResult = vCaptureInstance.iWICBitmapEncoder->CreateNewFrame(&vCaptureInstance.iWICBitmapFrameEncode, &vCaptureInstance.iPropertyBag2);
			if (FAILED(hResult))
			{
				CaptureResetVariablesBitmapImage();
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

				vCaptureInstance.iPropertyBag2->Write(1, &propertyValue, &variantValue);
			}

			//Initialize bitmap frame
			hResult = vCaptureInstance.iWICBitmapFrameEncode->Initialize(vCaptureInstance.iPropertyBag2);
			if (FAILED(hResult))
			{
				CaptureResetVariablesBitmapImage();
				return false;
			}

			//Bitmap frame set metadata
			if (SUCCEEDED(vCaptureInstance.iWICBitmapFrameEncode->GetMetadataQueryWriter(&vCaptureInstance.iWICMetadataQueryWriter)))
			{
				//Set application name
				PROPVARIANT propVariantAppName{};
				propVariantAppName.vt = VT_LPSTR;
				propVariantAppName.pszVal = "ScreenCaptureDLL";
				if (iWicFormatGuid == GUID_ContainerFormatPng)
				{
					vCaptureInstance.iWICMetadataQueryWriter->SetMetadataByName(L"/tEXt/{str=Software}", &propVariantAppName);
				}
				else
				{
					vCaptureInstance.iWICMetadataQueryWriter->SetMetadataByName(L"System.ApplicationName", &propVariantAppName);
				}

				//Set date taken
				FILETIME currentFileTime;
				SYSTEMTIME currentSystemTime;
				GetSystemTime(&currentSystemTime);
				SystemTimeToFileTime(&currentSystemTime, &currentFileTime);

				PROPVARIANT propVariantDateTaken{};
				propVariantDateTaken.vt = VT_FILETIME;
				propVariantDateTaken.filetime = currentFileTime;
				vCaptureInstance.iWICMetadataQueryWriter->SetMetadataByName(L"System.Photo.DateTaken", &propVariantDateTaken);
			}

			//Bitmap frame set size
			hResult = vCaptureInstance.iWICBitmapFrameEncode->SetSize(vCaptureDetails.OutputWidth, vCaptureDetails.OutputHeight);
			if (FAILED(hResult))
			{
				CaptureResetVariablesBitmapImage();
				return false;
			}

			std::cout << "Bitmap frame size, Width: " << vCaptureDetails.OutputWidth << " Height: " << vCaptureDetails.OutputHeight << std::endl;

			//Bitmap frame set resolution
			hResult = vCaptureInstance.iWICBitmapFrameEncode->SetResolution(96, 96);
			if (FAILED(hResult))
			{
				CaptureResetVariablesBitmapImage();
				return false;
			}

			//Write data to bitmap frame and convert jpg
			if (iWicFormatGuid == GUID_ContainerFormatJpeg && vCaptureInstance.iWicPixelFormatGuidSource != vCaptureInstance.iWicPixelFormatGuidJpeg)
			{
				hResult = vCaptureInstance.iWICImagingFactory->CreateBitmapFromMemory(vCaptureDetails.OutputWidth, vCaptureDetails.OutputHeight, vCaptureInstance.iWicPixelFormatGuidSource, vCaptureDetails.WidthByteSize, vCaptureDetails.TotalByteSize, bitmapData, &vCaptureInstance.iWICBitmap);
				if (FAILED(hResult))
				{
					CaptureResetVariablesBitmapImage();
					return false;
				}

				hResult = vCaptureInstance.iWICImagingFactory->CreateFormatConverter(&vCaptureInstance.iWICFormatConverter);
				if (FAILED(hResult))
				{
					CaptureResetVariablesBitmapImage();
					return false;
				}

				hResult = vCaptureInstance.iWICFormatConverter->Initialize(vCaptureInstance.iWICBitmap, vCaptureInstance.iWicPixelFormatGuidJpeg, WICBitmapDitherTypeNone, 0, 0, WICBitmapPaletteTypeCustom);
				if (FAILED(hResult))
				{
					CaptureResetVariablesBitmapImage();
					return false;
				}

				//Bitmap frame set pixelformat
				hResult = vCaptureInstance.iWICBitmapFrameEncode->SetPixelFormat(&vCaptureInstance.iWicPixelFormatGuidJpeg);
				if (FAILED(hResult))
				{
					CaptureResetVariablesBitmapImage();
					return false;
				}

				WICRect iWicRectangle = { 0, 0, (INT)vCaptureDetails.OutputWidth, (INT)vCaptureDetails.OutputHeight };
				hResult = vCaptureInstance.iWICBitmapFrameEncode->WriteSource(vCaptureInstance.iWICFormatConverter, &iWicRectangle);
				if (FAILED(hResult))
				{
					CaptureResetVariablesBitmapImage();
					return false;
				}
			}
			else
			{
				//Bitmap frame set pixelformat
				hResult = vCaptureInstance.iWICBitmapFrameEncode->SetPixelFormat(&vCaptureInstance.iWicPixelFormatGuidSource);
				if (FAILED(hResult))
				{
					CaptureResetVariablesBitmapImage();
					return false;
				}

				hResult = vCaptureInstance.iWICBitmapFrameEncode->WritePixels(vCaptureDetails.OutputHeight, vCaptureDetails.WidthByteSize, vCaptureDetails.TotalByteSize, bitmapData);
				if (FAILED(hResult))
				{
					CaptureResetVariablesBitmapImage();
					return false;
				}
			}

			//Commit bitmap frame
			hResult = vCaptureInstance.iWICBitmapFrameEncode->Commit();
			if (FAILED(hResult))
			{
				CaptureResetVariablesBitmapImage();
				return false;
			}

			//Commit bitmap encoder
			hResult = vCaptureInstance.iWICBitmapEncoder->Commit();
			if (FAILED(hResult))
			{
				CaptureResetVariablesBitmapImage();
				return false;
			}

			//Release resources
			CaptureResetVariablesBitmapImage();

			return true;
		}
		catch (...)
		{
			CaptureResetVariablesBitmapImage();
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

				std::cout << "Screen capture image bytes are empty." << std::endl;
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