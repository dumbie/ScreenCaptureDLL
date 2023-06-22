#pragma once
#include "CaptureVariables.h"
#include "CaptureReset.cpp"

namespace
{
	BOOL BitmapDataSaveFile(UINT captureInstanceId, BYTE* bitmapData, WCHAR* filePath, GUID iWicFormatGuid, UINT imageQuality)
	{
		try
		{
			//Create wic factory
			hResult = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_ALL, IID_IWICImagingFactory, (LPVOID*)&vCaptureInstances[captureInstanceId].iWICImagingFactory);
			if (FAILED(hResult))
			{
				CaptureResetVariablesBitmapImage(captureInstanceId);
				return false;
			}

			hResult = vCaptureInstances[captureInstanceId].iWICImagingFactory->CreateStream(&vCaptureInstances[captureInstanceId].iWICStream);
			if (FAILED(hResult))
			{
				CaptureResetVariablesBitmapImage(captureInstanceId);
				return false;
			}

			hResult = vCaptureInstances[captureInstanceId].iWICStream->InitializeFromFilename(filePath, GENERIC_WRITE);
			if (FAILED(hResult))
			{
				CaptureResetVariablesBitmapImage(captureInstanceId);
				return false;
			}

			//Create bitmap encoder
			hResult = vCaptureInstances[captureInstanceId].iWICImagingFactory->CreateEncoder(iWicFormatGuid, NULL, &vCaptureInstances[captureInstanceId].iWICBitmapEncoder);
			if (FAILED(hResult))
			{
				CaptureResetVariablesBitmapImage(captureInstanceId);
				return false;
			}

			hResult = vCaptureInstances[captureInstanceId].iWICBitmapEncoder->Initialize(vCaptureInstances[captureInstanceId].iWICStream, WICBitmapEncoderNoCache);
			if (FAILED(hResult))
			{
				CaptureResetVariablesBitmapImage(captureInstanceId);
				return false;
			}

			//Create bitmap frame
			hResult = vCaptureInstances[captureInstanceId].iWICBitmapEncoder->CreateNewFrame(&vCaptureInstances[captureInstanceId].iWICBitmapFrameEncode, &vCaptureInstances[captureInstanceId].iPropertyBag2);
			if (FAILED(hResult))
			{
				CaptureResetVariablesBitmapImage(captureInstanceId);
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

				vCaptureInstances[captureInstanceId].iPropertyBag2->Write(1, &propertyValue, &variantValue);
			}

			//Initialize bitmap frame
			hResult = vCaptureInstances[captureInstanceId].iWICBitmapFrameEncode->Initialize(vCaptureInstances[captureInstanceId].iPropertyBag2);
			if (FAILED(hResult))
			{
				CaptureResetVariablesBitmapImage(captureInstanceId);
				return false;
			}

			//Bitmap frame set metadata
			if (SUCCEEDED(vCaptureInstances[captureInstanceId].iWICBitmapFrameEncode->GetMetadataQueryWriter(&vCaptureInstances[captureInstanceId].iWICMetadataQueryWriter)))
			{
				//Set application name
				PROPVARIANT propVariantAppName{};
				propVariantAppName.vt = VT_LPSTR;
				propVariantAppName.pszVal = "ScreenCaptureDLL";
				if (iWicFormatGuid == GUID_ContainerFormatPng)
				{
					vCaptureInstances[captureInstanceId].iWICMetadataQueryWriter->SetMetadataByName(L"/tEXt/{str=Software}", &propVariantAppName);
				}
				else
				{
					vCaptureInstances[captureInstanceId].iWICMetadataQueryWriter->SetMetadataByName(L"System.ApplicationName", &propVariantAppName);
				}

				//Set date taken
				FILETIME currentFileTime;
				SYSTEMTIME currentSystemTime;
				GetSystemTime(&currentSystemTime);
				SystemTimeToFileTime(&currentSystemTime, &currentFileTime);

				PROPVARIANT propVariantDateTaken{};
				propVariantDateTaken.vt = VT_FILETIME;
				propVariantDateTaken.filetime = currentFileTime;
				vCaptureInstances[captureInstanceId].iWICMetadataQueryWriter->SetMetadataByName(L"System.Photo.DateTaken", &propVariantDateTaken);
			}

			//Bitmap frame set size
			hResult = vCaptureInstances[captureInstanceId].iWICBitmapFrameEncode->SetSize(vCaptureInstances[captureInstanceId].vCaptureDetails.OutputWidth, vCaptureInstances[captureInstanceId].vCaptureDetails.OutputHeight);
			if (FAILED(hResult))
			{
				CaptureResetVariablesBitmapImage(captureInstanceId);
				return false;
			}

			std::cout << "Bitmap frame size, Width: " << vCaptureInstances[captureInstanceId].vCaptureDetails.OutputWidth << " Height: " << vCaptureInstances[captureInstanceId].vCaptureDetails.OutputHeight << std::endl;

			//Bitmap frame set resolution
			hResult = vCaptureInstances[captureInstanceId].iWICBitmapFrameEncode->SetResolution(96, 96);
			if (FAILED(hResult))
			{
				CaptureResetVariablesBitmapImage(captureInstanceId);
				return false;
			}

			//Write data to bitmap frame and convert jpg
			if (iWicFormatGuid == GUID_ContainerFormatJpeg && vCaptureInstances[captureInstanceId].iWicPixelFormatGuidSource != vCaptureInstances[captureInstanceId].iWicPixelFormatGuidJpeg)
			{
				hResult = vCaptureInstances[captureInstanceId].iWICImagingFactory->CreateBitmapFromMemory(vCaptureInstances[captureInstanceId].vCaptureDetails.OutputWidth, vCaptureInstances[captureInstanceId].vCaptureDetails.OutputHeight, vCaptureInstances[captureInstanceId].iWicPixelFormatGuidSource, vCaptureInstances[captureInstanceId].vCaptureDetails.WidthByteSize, vCaptureInstances[captureInstanceId].vCaptureDetails.TotalByteSize, bitmapData, &vCaptureInstances[captureInstanceId].iWICBitmap);
				if (FAILED(hResult))
				{
					CaptureResetVariablesBitmapImage(captureInstanceId);
					return false;
				}

				hResult = vCaptureInstances[captureInstanceId].iWICImagingFactory->CreateFormatConverter(&vCaptureInstances[captureInstanceId].iWICFormatConverter);
				if (FAILED(hResult))
				{
					CaptureResetVariablesBitmapImage(captureInstanceId);
					return false;
				}

				hResult = vCaptureInstances[captureInstanceId].iWICFormatConverter->Initialize(vCaptureInstances[captureInstanceId].iWICBitmap, vCaptureInstances[captureInstanceId].iWicPixelFormatGuidJpeg, WICBitmapDitherTypeNone, 0, 0, WICBitmapPaletteTypeCustom);
				if (FAILED(hResult))
				{
					CaptureResetVariablesBitmapImage(captureInstanceId);
					return false;
				}

				//Bitmap frame set pixelformat
				hResult = vCaptureInstances[captureInstanceId].iWICBitmapFrameEncode->SetPixelFormat(&vCaptureInstances[captureInstanceId].iWicPixelFormatGuidJpeg);
				if (FAILED(hResult))
				{
					CaptureResetVariablesBitmapImage(captureInstanceId);
					return false;
				}

				WICRect iWicRectangle = { 0, 0, (INT)vCaptureInstances[captureInstanceId].vCaptureDetails.OutputWidth, (INT)vCaptureInstances[captureInstanceId].vCaptureDetails.OutputHeight };
				hResult = vCaptureInstances[captureInstanceId].iWICBitmapFrameEncode->WriteSource(vCaptureInstances[captureInstanceId].iWICFormatConverter, &iWicRectangle);
				if (FAILED(hResult))
				{
					CaptureResetVariablesBitmapImage(captureInstanceId);
					return false;
				}
			}
			else
			{
				//Bitmap frame set pixelformat
				hResult = vCaptureInstances[captureInstanceId].iWICBitmapFrameEncode->SetPixelFormat(&vCaptureInstances[captureInstanceId].iWicPixelFormatGuidSource);
				if (FAILED(hResult))
				{
					CaptureResetVariablesBitmapImage(captureInstanceId);
					return false;
				}

				hResult = vCaptureInstances[captureInstanceId].iWICBitmapFrameEncode->WritePixels(vCaptureInstances[captureInstanceId].vCaptureDetails.OutputHeight, vCaptureInstances[captureInstanceId].vCaptureDetails.WidthByteSize, vCaptureInstances[captureInstanceId].vCaptureDetails.TotalByteSize, bitmapData);
				if (FAILED(hResult))
				{
					CaptureResetVariablesBitmapImage(captureInstanceId);
					return false;
				}
			}

			//Commit bitmap frame
			hResult = vCaptureInstances[captureInstanceId].iWICBitmapFrameEncode->Commit();
			if (FAILED(hResult))
			{
				CaptureResetVariablesBitmapImage(captureInstanceId);
				return false;
			}

			//Commit bitmap encoder
			hResult = vCaptureInstances[captureInstanceId].iWICBitmapEncoder->Commit();
			if (FAILED(hResult))
			{
				CaptureResetVariablesBitmapImage(captureInstanceId);
				return false;
			}

			//Release resources
			CaptureResetVariablesBitmapImage(captureInstanceId);

			return true;
		}
		catch (...)
		{
			CaptureResetVariablesBitmapImage(captureInstanceId);
			return false;
		}
	}
};