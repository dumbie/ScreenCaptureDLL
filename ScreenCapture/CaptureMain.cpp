#pragma once
#include "CaptureVariables.h"
#include "CaptureDetails.cpp"
#include "CaptureImage.cpp"
#include "CaptureVideo.cpp"
#include "CaptureScreen.cpp"
#include "CaptureInitialize.cpp"
#include "CaptureTexture.cpp"

namespace
{
	extern "C"
	{
		__declspec(dllexport) BOOL CaptureInitialize(CaptureSettings captureSettings, CaptureDetails* captureDetails)
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

		__declspec(dllexport) BOOL CaptureFreeMemory(BYTE* bitmapData)
		{
			try
			{
				delete[] bitmapData;
				return true;
			}
			catch (...)
			{
				return false;
			}
		}

		__declspec(dllexport) BOOL CaptureSaveFileBmp(BYTE* bitmapData, WCHAR* filePath)
		{
			try
			{
				if (vCaptureDetails.HDREnabled && !vCaptureSettings.HDRtoSDR) { return false; }
				return BitmapDataSaveFile(bitmapData, filePath, GUID_ContainerFormatBmp);
			}
			catch (...)
			{
				return false;
			}
		}

		__declspec(dllexport) BOOL CaptureSaveFileJpg(BYTE* bitmapData, WCHAR* filePath, UINT imageQualityPercentage)
		{
			try
			{
				if (vCaptureDetails.HDREnabled && !vCaptureSettings.HDRtoSDR) { return false; }
				vBitmapImageQuality = imageQualityPercentage;
				return BitmapDataSaveFile(bitmapData, filePath, GUID_ContainerFormatJpeg);
			}
			catch (...)
			{
				return false;
			}
		}

		__declspec(dllexport) BOOL CaptureSaveFilePng(BYTE* bitmapData, WCHAR* filePath)
		{
			try
			{
				if (vCaptureDetails.HDREnabled && !vCaptureSettings.HDRtoSDR) { return false; }
				return BitmapDataSaveFile(bitmapData, filePath, GUID_ContainerFormatPng);
			}
			catch (...)
			{
				return false;
			}
		}

		__declspec(dllexport) BOOL CaptureSaveFileJxr(BYTE* bitmapData, WCHAR* filePath, UINT imageQualityPercentage)
		{
			try
			{
				vBitmapImageQuality = imageQualityPercentage;
				return BitmapDataSaveFile(bitmapData, filePath, GUID_ContainerFormatWmp);
			}
			catch (...)
			{
				return false;
			}
		}

		__declspec(dllexport) BOOL CaptureSaveFileTif(BYTE* bitmapData, WCHAR* filePath)
		{
			try
			{
				if (vCaptureDetails.HDREnabled && !vCaptureSettings.HDRtoSDR) { return false; }
				return BitmapDataSaveFile(bitmapData, filePath, GUID_ContainerFormatTiff);
			}
			catch (...)
			{
				return false;
			}
		}

		__declspec(dllexport) BOOL CaptureSaveFileHeif(BYTE* bitmapData, WCHAR* filePath, UINT imageQualityPercentage)
		{
			try
			{
				if (vCaptureDetails.HDREnabled && !vCaptureSettings.HDRtoSDR) { return false; }
				vBitmapImageQuality = imageQualityPercentage;
				return BitmapDataSaveFile(bitmapData, filePath, GUID_ContainerFormatHeif);
			}
			catch (...)
			{
				return false;
			}
		}

		__declspec(dllexport) BYTE* CaptureScreenshot()
		{
			return CaptureScreenBytes();
		}

		__declspec(dllexport) BOOL CaptureVideoStart()
		{
			try
			{
				InitializeMediaFoundation();
				return true;
			}
			catch (...)
			{
				CaptureResetVariablesTexture();
				CaptureResetVariablesVideo();
				return false;
			}
		}

		__declspec(dllexport) BOOL CaptureVideoStop()
		{
			try
			{
				//Stop video capture loop
				vVideoWriteLoop = false;

				//Release resources
				//CaptureResetVariablesTexture();
				//CaptureResetVariablesVideo();

				return true;
			}
			catch (...)
			{
				CaptureResetVariablesTexture();
				CaptureResetVariablesVideo();
				return false;
			}
		}
	}
};