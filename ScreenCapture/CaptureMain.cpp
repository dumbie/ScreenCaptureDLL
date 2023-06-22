#pragma once
#include "CaptureClasses.h"
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
		__declspec(dllexport) BOOL CaptureInitialize(UINT captureInstanceId, CaptureSettings captureSettings, CaptureDetails& captureDetails)
		{
			try
			{
				//Initialize capture
				BOOL captureInitialized = InitializeCapture(captureInstanceId, captureSettings, captureDetails);

				//Play sound effect
				if (!captureInitialized)
				{
					PlayAudio(captureInstanceId, L"Assets\\Capture\\CaptureFailed.mp3");
				}

				//Return result
				return captureInitialized;
			}
			catch (...)
			{
				return false;
			}
		}

		__declspec(dllexport) BOOL CaptureUpdateSettings(UINT captureInstanceId, CaptureSettings captureSettings)
		{
			try
			{
				//Update capture settings
				vCaptureInstances[captureInstanceId].vCaptureSettings = captureSettings;

				//Set shader variables
				return SetShaderVariables(captureInstanceId);
			}
			catch (...)
			{
				return false;
			}
		}

		__declspec(dllexport) BOOL CaptureReset(UINT captureInstanceId)
		{
			try
			{
				return CaptureResetVariablesAll(captureInstanceId);
			}
			catch (...)
			{
				return false;
			}
		}

		__declspec(dllexport) BYTE* CaptureScreenBytes(UINT captureInstanceId)
		{
			try
			{
				//Clear screen bytes cache
				vCaptureInstances[captureInstanceId].vScreenBytesCache.clear();

				//Update screen bytes cache
				vCaptureInstances[captureInstanceId].vScreenBytesCache = GetScreenBytes(captureInstanceId, true, false);

				//Return result
				return vCaptureInstances[captureInstanceId].vScreenBytesCache.data();
			}
			catch (...)
			{
				return NULL;
			}
		}

		__declspec(dllexport) BOOL CaptureImage(UINT captureInstanceId, WCHAR* filePath, UINT imageQuality, ImageFormats imageFormat)
		{
			try
			{
				//Get screen bytes
				std::vector<BYTE> screenBytes = GetScreenBytes(captureInstanceId, true, false);

				//Check screen bytes
				if (screenBytes.empty())
				{
					PlayAudio(captureInstanceId, L"Assets\\Capture\\CaptureFailed.mp3");
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
					if (vCaptureInstances[captureInstanceId].vCaptureDetails.HDREnabled && !vCaptureInstances[captureInstanceId].vCaptureDetails.HDRtoSDR) { return false; }
					imageSaveFormat = GUID_ContainerFormatJpeg;
				}
				else if (imageFormat == PNG)
				{
					if (vCaptureInstances[captureInstanceId].vCaptureDetails.HDREnabled && !vCaptureInstances[captureInstanceId].vCaptureDetails.HDRtoSDR) { return false; }
					imageSaveFormat = GUID_ContainerFormatPng;
				}
				else if (imageFormat == BMP)
				{
					if (vCaptureInstances[captureInstanceId].vCaptureDetails.HDREnabled && !vCaptureInstances[captureInstanceId].vCaptureDetails.HDRtoSDR) { return false; }
					imageSaveFormat = GUID_ContainerFormatBmp;
				}
				else if (imageFormat == TIF)
				{
					if (vCaptureInstances[captureInstanceId].vCaptureDetails.HDREnabled && !vCaptureInstances[captureInstanceId].vCaptureDetails.HDRtoSDR) { return false; }
					imageSaveFormat = GUID_ContainerFormatTiff;
				}
				else if (imageFormat == HEIF)
				{
					if (vCaptureInstances[captureInstanceId].vCaptureDetails.HDREnabled && !vCaptureInstances[captureInstanceId].vCaptureDetails.HDRtoSDR) { return false; }
					imageSaveFormat = GUID_ContainerFormatHeif;
				}

				//Save bitmap data to file
				BOOL savedBitmap = BitmapDataSaveFile(captureInstanceId, screenBytes.data(), filePath, imageSaveFormat, imageQuality);

				//Play sound effect
				if (savedBitmap)
				{
					PlayAudio(captureInstanceId, L"Assets\\Capture\\CaptureScreenshot.mp3");
				}
				else
				{
					PlayAudio(captureInstanceId, L"Assets\\Capture\\CaptureFailed.mp3");
				}

				//Return result
				return savedBitmap;
			}
			catch (...)
			{
				return false;
			}
		}

		__declspec(dllexport) BOOL CaptureVideoStart(UINT captureInstanceId, WCHAR* filePath, MediaSettings mediaSettings)
		{
			try
			{
				if (vCaptureInstances[captureInstanceId].vMediaCapturing) { return false; }

				//Update media settings
				vCaptureInstances[captureInstanceId].vMediaSettings = mediaSettings;

				//Start video capture loop
				vCaptureInstances[captureInstanceId].vMediaCapturing = true;
				vCaptureInstances[captureInstanceId].vMediaWriteLoopAllowed = true;
				vCaptureInstances[captureInstanceId].vMediaWriteLoopFinishedScreen = false;
				vCaptureInstances[captureInstanceId].vMediaWriteLoopFinishedAudio = false;

				//Initialize DXGI device manager
				bool initializeDXGI = InitializeDxgiDeviceManager(captureInstanceId);
				if (!initializeDXGI)
				{
					CaptureResetVariablesMedia(captureInstanceId);
					return false;
				}

				//Initialize media foundation
				bool initializeMedia = InitializeMediaFoundation(captureInstanceId, filePath);
				if (!initializeMedia)
				{
					CaptureResetVariablesMedia(captureInstanceId);
					return false;
				}

				//Begin media write
				hResult = vCaptureInstances[captureInstanceId].imfSinkWriter->BeginWriting();
				if (FAILED(hResult))
				{
					CaptureResetVariablesMedia(captureInstanceId);
					return false;
				}

				//Set media loop start time
				LARGE_INTEGER qpcTimeCurrent;
				QueryPerformanceCounter(&qpcTimeCurrent);
				vCaptureInstances[captureInstanceId].vMediaTimeStartLoop = qpcTimeCurrent.QuadPart;

				//Loop media write screen
				std::thread threadLoopWriteScreen(LoopWriteScreen, captureInstanceId);
				threadLoopWriteScreen.detach();

				//Loop media write audio
				std::thread threadLoopWriteAudio(LoopWriteAudio, captureInstanceId);
				threadLoopWriteAudio.detach();

				//Play audio effect
				PlayAudio(captureInstanceId, L"Assets\\Capture\\CaptureVideoStart.mp3");

				return true;
			}
			catch (...)
			{
				CaptureResetVariablesTexture(captureInstanceId);
				CaptureResetVariablesMedia(captureInstanceId);
				return false;
			}
		}

		__declspec(dllexport) BOOL CaptureVideoStop(UINT captureInstanceId)
		{
			try
			{
				//Stop video capture loop
				vCaptureInstances[captureInstanceId].vMediaCapturing = false;
				vCaptureInstances[captureInstanceId].vMediaWriteLoopAllowed = false;

				//Wait for loop to finish
				while (!vCaptureInstances[captureInstanceId].vMediaWriteLoopFinishedScreen && !vCaptureInstances[captureInstanceId].vMediaWriteLoopFinishedAudio)
				{
					std::cout << "Waiting for video capture loop to stop..." << std::endl;
					Sleep(1000);
				}

				//Finalize media write
				hResult = vCaptureInstances[captureInstanceId].imfSinkWriter->Finalize();
				if (FAILED(hResult))
				{
					std::cout << "Failed to finalize media capture..." << std::endl;
				}

				//Shutdown media foundation
				MFShutdown();

				//Release resources
				CaptureResetVariablesTexture(captureInstanceId);
				CaptureResetVariablesMedia(captureInstanceId);

				//Play audio effect
				PlayAudio(captureInstanceId, L"Assets\\Capture\\CaptureVideoStop.mp3");

				return true;
			}
			catch (...)
			{
				CaptureResetVariablesTexture(captureInstanceId);
				CaptureResetVariablesMedia(captureInstanceId);
				return false;
			}
		}

		__declspec(dllexport) BOOL CaptureVideoIsRecording(UINT captureInstanceId)
		{
			return vCaptureInstances[captureInstanceId].vMediaCapturing;
		}
	}
};