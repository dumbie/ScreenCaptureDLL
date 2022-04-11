#pragma once
#include "CaptureVariables.h"
#include "CaptureReset.cpp"

namespace
{
	BOOL BitmapDataSaveFileBmp(BYTE* bitmapData, WCHAR* filePath)
	{
		try
		{
			//Create bitmap
			Gdiplus::Bitmap gdiBitmap(vCaptureWidth, vCaptureHeight);
			Gdiplus::Rect nodRectangle(0, 0, vCaptureWidth, vCaptureHeight);

			//Set bitmap metadata
			Gdiplus::PropertyItem* propertyItem = new Gdiplus::PropertyItem;
			propertyItem->id = PropertyTagSoftwareUsed;
			propertyItem->length = 17;
			propertyItem->type = PropertyTagTypeASCII;
			propertyItem->value = "ScreenCaptureDLL";
			gdiBitmap.SetPropertyItem(propertyItem);

			//Copy data to bitmap
			Gdiplus::BitmapData* bitmapDataLock = new Gdiplus::BitmapData;
			gdiBitmap.LockBits(&nodRectangle, 0, PixelFormat32bppARGB, bitmapDataLock);
			memcpy(bitmapDataLock->Scan0, bitmapData, (size_t)vCaptureTotalByteSize);
			gdiBitmap.UnlockBits(bitmapDataLock);

			//Get save class identifier
			CLSID imageSaveClassId;
			hResult = CLSIDFromString(L"{557CF400-1A04-11D3-9A73-0000F81EF32E}", &imageSaveClassId);
			if (FAILED(hResult))
			{
				return false;
			}

			//Save bitmap to file
			gdiBitmap.Save(filePath, &imageSaveClassId, NULL);

			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	BOOL BitmapDataSaveFileJpg(BYTE* bitmapData, WCHAR* filePath, UINT saveQuality)
	{
		try
		{
			//Create bitmap
			Gdiplus::Bitmap gdiBitmap(vCaptureWidth, vCaptureHeight);
			Gdiplus::Rect nodRectangle(0, 0, vCaptureWidth, vCaptureHeight);

			//Set bitmap metadata
			Gdiplus::PropertyItem* propertyItem = new Gdiplus::PropertyItem;
			propertyItem->id = PropertyTagSoftwareUsed;
			propertyItem->length = 17;
			propertyItem->type = PropertyTagTypeASCII;
			propertyItem->value = "ScreenCaptureDLL";
			gdiBitmap.SetPropertyItem(propertyItem);

			//Copy data to bitmap
			Gdiplus::BitmapData* bitmapDataLock = new Gdiplus::BitmapData;
			gdiBitmap.LockBits(&nodRectangle, 0, PixelFormat32bppARGB, bitmapDataLock);
			memcpy(bitmapDataLock->Scan0, bitmapData, (size_t)vCaptureTotalByteSize);
			gdiBitmap.UnlockBits(bitmapDataLock);

			//Get save class identifier
			CLSID imageSaveClassId;
			hResult = CLSIDFromString(L"{557CF401-1A04-11D3-9A73-0000F81EF32E}", &imageSaveClassId);
			if (FAILED(hResult))
			{
				return false;
			}

			//Set bitmap parameters
			Gdiplus::EncoderParameters encoderParameters{};
			encoderParameters.Count = 1;
			encoderParameters.Parameter[0].Guid = Gdiplus::EncoderQuality;
			encoderParameters.Parameter[0].Type = Gdiplus::EncoderParameterValueTypeLong;
			encoderParameters.Parameter[0].NumberOfValues = 1;
			encoderParameters.Parameter[0].Value = &saveQuality;

			//Save bitmap to file
			gdiBitmap.Save(filePath, &imageSaveClassId, &encoderParameters);

			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	BOOL BitmapDataSaveFilePng(BYTE* bitmapData, WCHAR* filePath)
	{
		try
		{
			//Create bitmap
			Gdiplus::Bitmap gdiBitmap(vCaptureWidth, vCaptureHeight);
			Gdiplus::Rect nodRectangle(0, 0, vCaptureWidth, vCaptureHeight);
			Gdiplus::Graphics gdiGraphics(&gdiBitmap);

			//Set bitmap metadata
			Gdiplus::PropertyItem* propertyItem = new Gdiplus::PropertyItem;
			propertyItem->id = PropertyTagSoftwareUsed;
			propertyItem->length = 17;
			propertyItem->type = PropertyTagTypeASCII;
			propertyItem->value = "ScreenCaptureDLL";
			gdiBitmap.SetPropertyItem(propertyItem);

			//Copy data to bitmap
			Gdiplus::BitmapData* bitmapDataLock = new Gdiplus::BitmapData;
			gdiBitmap.LockBits(&nodRectangle, 0, PixelFormat32bppARGB, bitmapDataLock);
			memcpy(bitmapDataLock->Scan0, bitmapData, (size_t)vCaptureTotalByteSize);
			gdiBitmap.UnlockBits(bitmapDataLock);

			//Update alpha channel
			Gdiplus::ColorMatrix gdiColorMatrix =
			{
				1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 255.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 0.0f, 1.0f
			};
			Gdiplus::ImageAttributes gdiImageAttributes;
			gdiImageAttributes.SetColorMatrix(&gdiColorMatrix);
			gdiGraphics.DrawImage(&gdiBitmap, nodRectangle, 0, 0, vCaptureWidth, vCaptureHeight, Gdiplus::UnitPixel, &gdiImageAttributes);

			//Get save class identifier
			CLSID imageSaveClassId;
			hResult = CLSIDFromString(L"{557CF406-1A04-11D3-9A73-0000F81EF32E}", &imageSaveClassId);
			if (FAILED(hResult))
			{
				return false;
			}

			//Save bitmap to file
			gdiBitmap.Save(filePath, &imageSaveClassId, NULL);

			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	BOOL BitmapDataSaveFileJxr(BYTE* bitmapData, WCHAR* filePath)
	{
		try
		{
			//Set target file format
			GUID iWicFormatGuid = GUID_ContainerFormatWmp;

			//Set target pixel format
			WICPixelFormatGUID iWicPixelFormatGuid = GUID_WICPixelFormat32bppBGRA;
			if (vCaptureHDREnabled && !vCaptureHDRtoSDR)
			{
				iWicPixelFormatGuid = GUID_WICPixelFormat64bppRGBAHalf;
			}

			//Create wicfactory
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
				PROPVARIANT propVariant;
				PropVariantInit(&propVariant);
				propVariant.vt = VT_LPSTR;
				propVariant.pszVal = "ScreenCaptureDLL";

				//Set application name
				hResult = iWICMetadataQueryWriter->SetMetadataByName(L"System.ApplicationName", &propVariant);
				if (FAILED(hResult))
				{
					CaptureResetVariablesBitmap();
					return false;
				}
			}

			//Bitmap frame set size
			hResult = iWICBitmapFrameEncode->SetSize(vCaptureWidth, vCaptureHeight);
			if (FAILED(hResult))
			{
				CaptureResetVariablesBitmap();
				return false;
			}

			//Bitmap frame set pixelformat
			hResult = iWICBitmapFrameEncode->SetPixelFormat(&iWicPixelFormatGuid);
			if (FAILED(hResult))
			{
				CaptureResetVariablesBitmap();
				return false;
			}

			//Write data to bitmap frame
			hResult = iWICBitmapFrameEncode->WritePixels(vCaptureHeight, vCaptureWidthByteSize, vCaptureTotalByteSize, bitmapData);
			if (FAILED(hResult))
			{
				CaptureResetVariablesBitmap();
				return false;
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