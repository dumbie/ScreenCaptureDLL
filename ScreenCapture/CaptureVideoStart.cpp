#pragma once
#include "CaptureVariables.h"
#include "CaptureInitMedia.cpp"
#include "CaptureLoop.cpp"
#include "CaptureReset.cpp"
#include "PlayAudio.cpp"

namespace
{
	BOOL CaptureVideoStartCode(WCHAR* filePath, MediaSettings mediaSettings)
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
				CaptureResetVariablesMediaAll();
				return false;
			}

			//Initialize media foundation
			bool initializeMedia = InitializeMediaFoundation(filePath);
			if (!initializeMedia)
			{
				CaptureResetVariablesMediaAll();
				return false;
			}

			//Begin media write
			hResult = vCaptureInstance.imfSinkWriter->BeginWriting();
			if (FAILED(hResult))
			{
				CaptureResetVariablesMediaAll();
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
			CaptureResetVariablesMediaAll();
			return false;
		}
	}
}