#pragma once
#include "CaptureVariables.h"
#include "CaptureDetails.cpp"
#include "CaptureImage.cpp"
#include "CaptureMedia.cpp"
#include "CaptureScreen.cpp"
#include "CaptureInitialize.cpp"
#include "CaptureTexture.cpp"
#include "CaptureLoop.cpp"

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

		__declspec(dllexport) BYTE* CaptureScreenBytes()
		{
			try
			{
				//Clear screen bytes cache
				vScreenBytesCacheCapture.clear();

				//Update screen bytes cache
				vScreenBytesCacheCapture = GetScreenBytes(true, false);

				//Return result
				return vScreenBytesCacheCapture.data();
			}
			catch (...)
			{
				return NULL;
			}
		}

		__declspec(dllexport) BOOL CaptureImage(WCHAR* filePath, UINT imageQuality, ImageFormats imageFormat)
		{
			try
			{
				//Get screen bytes
				std::vector<BYTE> screenBytes = GetScreenBytes(true, false);

				//Check screen bytes
				if (screenBytes.empty())
				{
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
				return BitmapDataSaveFile(screenBytes.data(), filePath, imageSaveFormat, imageQuality);
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
				vMediaWriteLoopFinishedScreen = false;
				vMediaWriteLoopFinishedAudio = false;

				//Initialize media foundation
				bool initializeMedia = InitializeMediaFoundation(filePath);
				if (!initializeMedia)
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

				//Set media loop start time
				LARGE_INTEGER qpcTimeCurrent;
				QueryPerformanceCounter(&qpcTimeCurrent);
				vMediaTimeStartLoop = qpcTimeCurrent.QuadPart;

				//Loop media write screen
				std::thread threadLoopWriteScreen(LoopWriteScreen);
				threadLoopWriteScreen.detach();

				//Loop media write audio
				std::thread threadLoopWriteAudio(LoopWriteAudio);
				threadLoopWriteAudio.detach();

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
				while (!vMediaWriteLoopFinishedScreen && !vMediaWriteLoopFinishedAudio) { Sleep(100); }

				//Finalize media write
				hResult = imfSinkWriter->Finalize();

				//Shutdown media foundation
				MFShutdown();

				//CoUninitialize
				CoUninitialize();

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

		__declspec(dllexport) BOOL CaptureVideoIsRecording()
		{
			return vMediaCapturing;
		}
	}
};