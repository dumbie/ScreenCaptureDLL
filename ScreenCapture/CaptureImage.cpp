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
			hResult = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_ALL, IID_IWICImagingFactory, (LPVOID*)&iWICImagingFactory);
			if (FAILED(hResult))
			{
				CaptureResetVariablesBitmapImage();
				return false;
			}

			hResult = iWICImagingFactory->CreateStream(&iWICStream);
			if (FAILED(hResult))
			{
				CaptureResetVariablesBitmapImage();
				return false;
			}

			hResult = iWICStream->InitializeFromFilename(filePath, GENERIC_WRITE);
			if (FAILED(hResult))
			{
				CaptureResetVariablesBitmapImage();
				return false;
			}

			//Create bitmap encoder
			hResult = iWICImagingFactory->CreateEncoder(iWicFormatGuid, NULL, &iWICBitmapEncoder);
			if (FAILED(hResult))
			{
				CaptureResetVariablesBitmapImage();
				return false;
			}

			hResult = iWICBitmapEncoder->Initialize(iWICStream, WICBitmapEncoderNoCache);
			if (FAILED(hResult))
			{
				CaptureResetVariablesBitmapImage();
				return false;
			}

			//Create bitmap frame
			hResult = iWICBitmapEncoder->CreateNewFrame(&iWICBitmapFrameEncode, &iPropertyBag2);
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

				iPropertyBag2->Write(1, &propertyValue, &variantValue);
			}

			//Initialize bitmap frame
			hResult = iWICBitmapFrameEncode->Initialize(iPropertyBag2);
			if (FAILED(hResult))
			{
				CaptureResetVariablesBitmapImage();
				return false;
			}

			//Bitmap frame set metadata
			if (SUCCEEDED(iWICBitmapFrameEncode->GetMetadataQueryWriter(&iWICMetadataQueryWriter)))
			{
				//Set application name
				PROPVARIANT propVariantAppName{};
				propVariantAppName.vt = VT_LPSTR;
				propVariantAppName.pszVal = "ScreenCaptureDLL";
				if (iWicFormatGuid == GUID_ContainerFormatPng)
				{
					iWICMetadataQueryWriter->SetMetadataByName(L"/tEXt/{str=Software}", &propVariantAppName);
				}
				else
				{
					iWICMetadataQueryWriter->SetMetadataByName(L"System.ApplicationName", &propVariantAppName);
				}

				//Set date taken
				FILETIME currentFileTime;
				SYSTEMTIME currentSystemTime;
				GetSystemTime(&currentSystemTime);
				SystemTimeToFileTime(&currentSystemTime, &currentFileTime);

				PROPVARIANT propVariantDateTaken{};
				propVariantDateTaken.vt = VT_FILETIME;
				propVariantDateTaken.filetime = currentFileTime;
				iWICMetadataQueryWriter->SetMetadataByName(L"System.Photo.DateTaken", &propVariantDateTaken);
			}

			//Bitmap frame set size
			hResult = iWICBitmapFrameEncode->SetSize(vCaptureDetails.Width, vCaptureDetails.Height);
			if (FAILED(hResult))
			{
				CaptureResetVariablesBitmapImage();
				return false;
			}

			//Bitmap frame set resolution
			hResult = iWICBitmapFrameEncode->SetResolution(96, 96);
			if (FAILED(hResult))
			{
				CaptureResetVariablesBitmapImage();
				return false;
			}

			//Write data to bitmap frame and convert jpg
			if (iWicFormatGuid == GUID_ContainerFormatJpeg && iWicPixelFormatGuidSource != iWicPixelFormatGuidJpeg)
			{
				hResult = iWICImagingFactory->CreateBitmapFromMemory(vCaptureDetails.Width, vCaptureDetails.Height, iWicPixelFormatGuidSource, vCaptureDetails.WidthByteSize, vCaptureDetails.TotalByteSize, bitmapData, &iWICBitmap);
				if (FAILED(hResult))
				{
					CaptureResetVariablesBitmapImage();
					return false;
				}

				hResult = iWICImagingFactory->CreateFormatConverter(&iWICFormatConverter);
				if (FAILED(hResult))
				{
					CaptureResetVariablesBitmapImage();
					return false;
				}

				hResult = iWICFormatConverter->Initialize(iWICBitmap, iWicPixelFormatGuidJpeg, WICBitmapDitherTypeNone, 0, 0, WICBitmapPaletteTypeCustom);
				if (FAILED(hResult))
				{
					CaptureResetVariablesBitmapImage();
					return false;
				}

				//Bitmap frame set pixelformat
				hResult = iWICBitmapFrameEncode->SetPixelFormat(&iWicPixelFormatGuidJpeg);
				if (FAILED(hResult))
				{
					CaptureResetVariablesBitmapImage();
					return false;
				}

				WICRect iWicRectangle = { 0, 0, (INT)vCaptureDetails.Width, (INT)vCaptureDetails.Height };
				hResult = iWICBitmapFrameEncode->WriteSource(iWICFormatConverter, &iWicRectangle);
				if (FAILED(hResult))
				{
					CaptureResetVariablesBitmapImage();
					return false;
				}
			}
			else
			{
				//Bitmap frame set pixelformat
				hResult = iWICBitmapFrameEncode->SetPixelFormat(&iWicPixelFormatGuidSource);
				if (FAILED(hResult))
				{
					CaptureResetVariablesBitmapImage();
					return false;
				}

				hResult = iWICBitmapFrameEncode->WritePixels(vCaptureDetails.Height, vCaptureDetails.WidthByteSize, vCaptureDetails.TotalByteSize, bitmapData);
				if (FAILED(hResult))
				{
					CaptureResetVariablesBitmapImage();
					return false;
				}
			}

			//Commit bitmap frame
			hResult = iWICBitmapFrameEncode->Commit();
			if (FAILED(hResult))
			{
				CaptureResetVariablesBitmapImage();
				return false;
			}

			//Commit bitmap encoder
			hResult = iWICBitmapEncoder->Commit();
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