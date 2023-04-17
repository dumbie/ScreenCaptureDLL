#pragma once
#include "CaptureVariables.h"
#include "CaptureDetails.cpp"
#include "CaptureImage.cpp"
#include "CaptureMedia.cpp"
#include "CaptureScreen.cpp"
#include "CaptureInitialize.cpp"
#include "CaptureTexture.cpp"

namespace
{
	extern "C"
	{
		__declspec(dllexport) BOOL CaptureInitialize(CaptureSettings captureSettings, CaptureDetails& captureDetails)
		{
			try
			{
				return InitializeCapture(captureSettings, captureDetails);
			}
			catch (...)
			{
				return false;
			}
		}

		__declspec(dllexport) BOOL CaptureUpdateSettings(CaptureSettings captureSettings)
		{
			try
			{
				//Update capture settings
				vCaptureSettings = captureSettings;

				//Set shader variables
				return SetShaderVariables();
			}
			catch (...)
			{
				return false;
			}
		}

		__declspec(dllexport) BOOL CaptureUpdateSDRWhiteLevel()
		{
			try
			{
				return UpdateMonitorSDRWhiteLevel();
			}
			catch (...)
			{
				return false;
			}
		}

		__declspec(dllexport) BOOL CaptureReset()
		{
			try
			{
				return CaptureResetVariablesAll();
			}
			catch (...)
			{
				return false;
			}
		}

		__declspec(dllexport) BYTE* CaptureScreenshot()
		{
			UpdateScreenBytesCache(true, false);
			return vScreenBytesCache.data();
		}

		__declspec(dllexport) BOOL CaptureImage(BYTE* bitmapData, WCHAR* filePath, UINT imageQuality, ImageFormats imageFormat)
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
					if (vCaptureDetails.HDREnabled && !vCaptureSettings.HDRtoSDR) { return false; }
					imageSaveFormat = GUID_ContainerFormatJpeg;
				}
				else if (imageFormat == PNG)
				{
					if (vCaptureDetails.HDREnabled && !vCaptureSettings.HDRtoSDR) { return false; }
					imageSaveFormat = GUID_ContainerFormatPng;
				}
				else if (imageFormat == BMP)
				{
					if (vCaptureDetails.HDREnabled && !vCaptureSettings.HDRtoSDR) { return false; }
					imageSaveFormat = GUID_ContainerFormatBmp;
				}
				else if (imageFormat == TIF)
				{
					if (vCaptureDetails.HDREnabled && !vCaptureSettings.HDRtoSDR) { return false; }
					imageSaveFormat = GUID_ContainerFormatTiff;
				}
				else if (imageFormat == HEIF)
				{
					if (vCaptureDetails.HDREnabled && !vCaptureSettings.HDRtoSDR) { return false; }
					imageSaveFormat = GUID_ContainerFormatHeif;
				}

				//Save bitmap data to file
				return BitmapDataSaveFile(bitmapData, filePath, imageSaveFormat, imageQuality);
			}
			catch (...)
			{
				return false;
			}
		}

		__declspec(dllexport) BOOL CaptureVideoStart(WCHAR* filePath)
		{
			try
			{
				if (vMediaCapturing) { return false; }

				//Start video capture loop
				vMediaCapturing = true;
				vMediaWriteLoopAllowed = true;
				vMediaWriteLoopFinished = false;

				//Initialize media foundation
				bool initialized = InitializeMediaFoundation(filePath);
				if (!initialized)
				{
					CaptureResetVariablesMedia();
					return false;
				}

				//Begin media write
				hResult = imfSinkWriter->BeginWriting();
				if (FAILED(hResult))
				{
					CaptureResetVariablesMedia();
					return false;
				}

				//Loop media write
				std::thread threadLoopMedia(WriteMediaLoop, imfSinkWriter);
				threadLoopMedia.detach();

				return true;
			}
			catch (...)
			{
				CaptureResetVariablesTexture();
				CaptureResetVariablesMedia();
				return false;
			}
		}

		__declspec(dllexport) BOOL CaptureVideoStop()
		{
			try
			{
				//Stop video capture loop
				vMediaCapturing = false;
				vMediaWriteLoopAllowed = false;

				//Wait for loop to finish
				while (!vMediaWriteLoopFinished) { Sleep(100); }

				//Finalize media write
				hResult = imfSinkWriter->Finalize();

				//Release resources
				CaptureResetVariablesTexture();
				CaptureResetVariablesMedia();

				return true;
			}
			catch (...)
			{
				CaptureResetVariablesTexture();
				CaptureResetVariablesMedia();
				return false;
			}
		}
	}
};