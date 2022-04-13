#include "CaptureVariables.h"
#include "CaptureReset.h"

namespace
{
	BOOL BitmapDataSaveFile(BYTE* bitmapData, WCHAR* filePath, GUID iWicFormatGuid)
	{
		try
		{
			//Create wic factory
			hResult = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)&iWICImagingFactory);
			if (FAILED(hResult))
			{
				CaptureResetVariablesBitmap();
				return false;
			}

			hResult = iWICImagingFactory->CreateStream(&iWICStream);
			if (FAILED(hResult))
			{
				CaptureResetVariablesBitmap();
				return false;
			}

			hResult = iWICStream->InitializeFromFilename(filePath, GENERIC_WRITE);
			if (FAILED(hResult))
			{
				CaptureResetVariablesBitmap();
				return false;
			}

			//Create bitmap encoder
			hResult = iWICImagingFactory->CreateEncoder(iWicFormatGuid, NULL, &iWICBitmapEncoder);
			if (FAILED(hResult))
			{
				CaptureResetVariablesBitmap();
				return false;
			}

			hResult = iWICBitmapEncoder->Initialize(iWICStream, WICBitmapEncoderNoCache);
			if (FAILED(hResult))
			{
				CaptureResetVariablesBitmap();
				return false;
			}

			//Create bitmap frame
			hResult = iWICBitmapEncoder->CreateNewFrame(&iWICBitmapFrameEncode, &iPropertyBag2);
			if (FAILED(hResult))
			{
				CaptureResetVariablesBitmap();
				return false;
			}

			//Write property bag
			if (iWicFormatGuid == GUID_ContainerFormatJpeg)
			{
				PROPBAG2 propertyValue{};
				propertyValue.pstrName = L"ImageQuality";

				VARIANT variantValue{};
				variantValue.vt = VT_R4;
				variantValue.fltVal = vBitmapImageQuality / 100.0F;

				iPropertyBag2->Write(1, &propertyValue, &variantValue);
			}

			//Initialize bitmap frame
			hResult = iWICBitmapFrameEncode->Initialize(iPropertyBag2);
			if (FAILED(hResult))
			{
				CaptureResetVariablesBitmap();
				return false;
			}

			//Bitmap frame set metadata
			if (SUCCEEDED(iWICBitmapFrameEncode->GetMetadataQueryWriter(&iWICMetadataQueryWriter)))
			{
				PROPVARIANT propVariant{};
				propVariant.vt = VT_LPSTR;
				propVariant.pszVal = "ScreenCaptureDLL";

				//Set application name
				if (iWicFormatGuid == GUID_ContainerFormatPng)
				{
					hResult = iWICMetadataQueryWriter->SetMetadataByName(L"/tEXt/{str=Software}", &propVariant);
					if (FAILED(hResult))
					{
						CaptureResetVariablesBitmap();
						return false;
					}
				}
				else
				{
					hResult = iWICMetadataQueryWriter->SetMetadataByName(L"System.ApplicationName", &propVariant);
					if (FAILED(hResult))
					{
						CaptureResetVariablesBitmap();
						return false;
					}
				}
			}

			//Bitmap frame set size
			hResult = iWICBitmapFrameEncode->SetSize(vCaptureDetails.Width, vCaptureDetails.Height);
			if (FAILED(hResult))
			{
				CaptureResetVariablesBitmap();
				return false;
			}

			//Bitmap frame set resolution
			hResult = iWICBitmapFrameEncode->SetResolution(96, 96);
			if (FAILED(hResult))
			{
				CaptureResetVariablesBitmap();
				return false;
			}

			//Write data to bitmap frame and convert jpg
			if (iWicFormatGuid == GUID_ContainerFormatJpeg && iWicPixelFormatGuidSource != iWicPixelFormatGuidJpeg)
			{
				hResult = iWICImagingFactory->CreateBitmapFromMemory(vCaptureDetails.Width, vCaptureDetails.Height, iWicPixelFormatGuidSource, vCaptureDetails.WidthByteSize, vCaptureDetails.TotalByteSize, bitmapData, &iWICBitmap);
				if (FAILED(hResult))
				{
					CaptureResetVariablesBitmap();
					return false;
				}

				hResult = iWICImagingFactory->CreateFormatConverter(&iWICFormatConverter);
				if (FAILED(hResult))
				{
					CaptureResetVariablesBitmap();
					return false;
				}

				hResult = iWICFormatConverter->Initialize(iWICBitmap, iWicPixelFormatGuidJpeg, WICBitmapDitherTypeNone, 0, 0, WICBitmapPaletteTypeCustom);
				if (FAILED(hResult))
				{
					CaptureResetVariablesBitmap();
					return false;
				}

				//Bitmap frame set pixelformat
				hResult = iWICBitmapFrameEncode->SetPixelFormat(&iWicPixelFormatGuidJpeg);
				if (FAILED(hResult))
				{
					CaptureResetVariablesBitmap();
					return false;
				}

				WICRect iWicRectangle = { 0, 0, (INT)vCaptureDetails.Width, (INT)vCaptureDetails.Height };
				hResult = iWICBitmapFrameEncode->WriteSource(iWICFormatConverter, &iWicRectangle);
				if (FAILED(hResult))
				{
					CaptureResetVariablesBitmap();
					return false;
				}
			}
			else
			{
				//Bitmap frame set pixelformat
				hResult = iWICBitmapFrameEncode->SetPixelFormat(&iWicPixelFormatGuidSource);
				if (FAILED(hResult))
				{
					CaptureResetVariablesBitmap();
					return false;
				}

				hResult = iWICBitmapFrameEncode->WritePixels(vCaptureDetails.Height, vCaptureDetails.WidthByteSize, vCaptureDetails.TotalByteSize, bitmapData);
				if (FAILED(hResult))
				{
					CaptureResetVariablesBitmap();
					return false;
				}
			}

			//Commit bitmap frame
			hResult = iWICBitmapFrameEncode->Commit();
			if (FAILED(hResult))
			{
				CaptureResetVariablesBitmap();
				return false;
			}

			//Commit bitmap encoder
			hResult = iWICBitmapEncoder->Commit();
			if (FAILED(hResult))
			{
				CaptureResetVariablesBitmap();
				return false;
			}

			//Release resources
			CaptureResetVariablesBitmap();

			return true;
		}
		catch (...)
		{
			CaptureResetVariablesBitmap();
			return false;
		}
	}
};