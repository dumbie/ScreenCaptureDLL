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
		__declspec(dllexport) BOOL CaptureInitialize(CaptureSettings captureSettings, CaptureDetails& captureDetails, BOOL forceInitialize)
		{
			try
			{
				//Initialize capture
				BOOL captureInitialized = InitializeCapture(captureSettings, captureDetails, forceInitialize);

				//Check if capture initialized
				if (!captureInitialized)
				{
					//Reset all used variables
					DirectXResetVariablesAll();
					CaptureResetVariablesAll();

					//Play audio effect
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
				vCaptureInstance.vCaptureSettings = captureSettings;

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
				BOOL resetDirectX = DirectXResetVariablesAll();
				BOOL resetCapture = CaptureResetVariablesAll();
				return resetDirectX && resetCapture;
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
				vCaptureInstance.vScreenBytesCache.clear();

				//Update screen bytes cache
				vCaptureInstance.vScreenBytesCache = GetScreenBytes(true, false);

				//Return result
				return vCaptureInstance.vScreenBytesCache.data();
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
					//Play audio effect
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
					if (vCaptureInstance.vCaptureDetails.HDREnabled && !vCaptureInstance.vCaptureDetails.HDRtoSDR) { return false; }
					imageSaveFormat = GUID_ContainerFormatJpeg;
				}
				else if (imageFormat == PNG)
				{
					if (vCaptureInstance.vCaptureDetails.HDREnabled && !vCaptureInstance.vCaptureDetails.HDRtoSDR) { return false; }
					imageSaveFormat = GUID_ContainerFormatPng;
				}
				else if (imageFormat == BMP)
				{
					if (vCaptureInstance.vCaptureDetails.HDREnabled && !vCaptureInstance.vCaptureDetails.HDRtoSDR) { return false; }
					imageSaveFormat = GUID_ContainerFormatBmp;
				}
				else if (imageFormat == TIF)
				{
					if (vCaptureInstance.vCaptureDetails.HDREnabled && !vCaptureInstance.vCaptureDetails.HDRtoSDR) { return false; }
					imageSaveFormat = GUID_ContainerFormatTiff;
				}
				else if (imageFormat == HEIF)
				{
					if (vCaptureInstance.vCaptureDetails.HDREnabled && !vCaptureInstance.vCaptureDetails.HDRtoSDR) { return false; }
					imageSaveFormat = GUID_ContainerFormatHeif;
				}

				//Save bitmap data to file
				BOOL savedBitmap = BitmapDataSaveFile(screenBytes.data(), filePath, imageSaveFormat, imageQuality);

				//Play audio effect
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
				if (vCaptureInstance.vMediaCapturing) { return false; }

				//Update media settings
				vCaptureInstance.vMediaSettings = mediaSettings;

				//Start video capture loop
				vCaptureInstance.vMediaCapturing = true;
				vCaptureInstance.vMediaWriteLoopAllowed = true;
				vCaptureInstance.vMediaWriteLoopFinishedScreen = false;
				vCaptureInstance.vMediaWriteLoopFinishedAudio = false;

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
				hResult = vCaptureInstance.imfSinkWriter->BeginWriting();
				if (FAILED(hResult))
				{
					CaptureResetVariablesMedia();
					return false;
				}

				//Set media loop start time
				LARGE_INTEGER qpcTimeCurrent;
				QueryPerformanceCounter(&qpcTimeCurrent);
				vCaptureInstance.vMediaTimeStartLoop = qpcTimeCurrent.QuadPart;

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
				CaptureResetVariablesTexturesLoop();
				CaptureResetVariablesMedia();
				return false;
			}
		}

		__declspec(dllexport) BOOL CaptureVideoStop()
		{
			try
			{
				//Stop video capture loop
				vCaptureInstance.vMediaCapturing = false;
				vCaptureInstance.vMediaWriteLoopAllowed = false;

				//Wait for loop to finish
				while (!vCaptureInstance.vMediaWriteLoopFinishedScreen && !vCaptureInstance.vMediaWriteLoopFinishedAudio)
				{
					std::cout << "Waiting for video capture loop to stop..." << std::endl;
					Sleep(1000);
				}

				//Finalize media write
				hResult = vCaptureInstance.imfSinkWriter->Finalize();
				if (FAILED(hResult))
				{
					std::cout << "Failed to finalize media capture..." << std::endl;
				}

				//Shutdown media foundation
				MFShutdown();

				//Release resources
				CaptureResetVariablesTexturesLoop();
				CaptureResetVariablesMedia();

				//Play audio effect
				PlayAudio(L"Assets\\Capture\\CaptureVideoStop.mp3");

				return true;
			}
			catch (...)
			{
				CaptureResetVariablesTexturesLoop();
				CaptureResetVariablesMedia();
				return false;
			}
		}

		__declspec(dllexport) BOOL CaptureVideoIsRecording()
		{
			return vCaptureInstance.vMediaCapturing;
		}
	}
};