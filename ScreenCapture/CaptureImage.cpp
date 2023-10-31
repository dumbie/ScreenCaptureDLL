#pragma once
#include "CaptureVariables.h"
#include "CaptureReset.cpp"

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
			hResult = vCaptureInstance.iWICBitmapFrameEncode->SetSize(vCaptureInstance.vCaptureDetails.OutputWidth, vCaptureInstance.vCaptureDetails.OutputHeight);
			if (FAILED(hResult))
			{
				CaptureResetVariablesBitmapImage();
				return false;
			}

			std::cout << "Bitmap frame size, Width: " << vCaptureInstance.vCaptureDetails.OutputWidth << " Height: " << vCaptureInstance.vCaptureDetails.OutputHeight << std::endl;

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
				hResult = vCaptureInstance.iWICImagingFactory->CreateBitmapFromMemory(vCaptureInstance.vCaptureDetails.OutputWidth, vCaptureInstance.vCaptureDetails.OutputHeight, vCaptureInstance.iWicPixelFormatGuidSource, vCaptureInstance.vCaptureDetails.WidthByteSize, vCaptureInstance.vCaptureDetails.TotalByteSize, bitmapData, &vCaptureInstance.iWICBitmap);
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

				WICRect iWicRectangle = { 0, 0, (INT)vCaptureInstance.vCaptureDetails.OutputWidth, (INT)vCaptureInstance.vCaptureDetails.OutputHeight };
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

				hResult = vCaptureInstance.iWICBitmapFrameEncode->WritePixels(vCaptureInstance.vCaptureDetails.OutputHeight, vCaptureInstance.vCaptureDetails.WidthByteSize, vCaptureInstance.vCaptureDetails.TotalByteSize, bitmapData);
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
};