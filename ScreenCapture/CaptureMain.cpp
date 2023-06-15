#pragma once
#include "CaptureVariables.h"
#include "CaptureDetails.cpp"
#include "CaptureImage.cpp"
#include "CaptureMedia.cpp"
#include "CaptureScreen.cpp"
#include "CaptureInitialize.cpp"
#include "CaptureTexture.cpp"
#include "CaptureLoop.cpp"
#include "PlayAudio.cpp"

namespace
{
	extern "C"
	{
		__declspec(dllexport) BOOL CaptureInitialize(CaptureSettings captureSettings, CaptureDetails& captureDetails)
		{
			try
			{
				//Initialize capture
				BOOL captureInitialized = InitializeCapture(captureSettings, captureDetails);

				//Play sound effect
				if (!captureInitialized)
				{
					PlayAudio(L"Assets\\Capture\\CaptureFailed.mp3");
				}

				//Return result
				return captureInitialized;
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
				vScreenBytesCache.clear();

				//Update screen bytes cache
				vScreenBytesCache = GetScreenBytes(true, false, !vMediaCapturing);

				//Return result
				return vScreenBytesCache.data();
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
				std::vector<BYTE> screenBytes = GetScreenBytes(true, false, !vMediaCapturing);

				//Check screen bytes
				if (screenBytes.empty())
				{
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

				//Play sound effect
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

		__declspec(dllexport) BOOL CaptureVideoStart(WCHAR* filePath, MediaSettings mediaSettings)
		{
			try
			{
				if (vMediaCapturing) { return false; }

				//Update media settings
				vMediaSettings = mediaSettings;

				//Start video capture loop
				vMediaCapturing = true;
				vMediaWriteLoopAllowed = true;
				vMediaWriteLoopFinishedScreen = false;
				vMediaWriteLoopFinishedAudio = false;

				//Initialize DXGI device manager
				bool initializeDXGI = InitializeDxgiDeviceManager();
				if (!initializeDXGI)
				{
					CaptureResetVariablesMedia();
					return false;
				}

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

				//Play audio effect
				PlayAudio(L"Assets\\Capture\\CaptureVideoStart.mp3");

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
				while (!vMediaWriteLoopFinishedScreen && !vMediaWriteLoopFinishedAudio) { Sleep(500); }

				//Finalize media write
				hResult = imfSinkWriter->Finalize();

				//Shutdown media foundation
				MFShutdown();

				//Release resources
				CaptureResetVariablesTexture();
				CaptureResetVariablesMedia();

				//Play audio effect
				PlayAudio(L"Assets\\Capture\\CaptureVideoStop.mp3");

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