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
			//Check capture status
			if (vMediaFoundationInstance.vMediaCapturing) { return false; }

			//Update capture status
			vMediaFoundationInstance.vMediaCapturing = true;

			//Update media settings
			vMediaSettings = mediaSettings;

			//Initialize DXGI device manager
			bool initializeDXGI = InitializeDxgiDeviceManager();
			if (!initializeDXGI)
			{
				MediaFoundationResetVariablesAll();
				return false;
			}

			//Initialize media foundation
			bool initializeMedia = InitializeMediaFoundation(filePath);
			if (!initializeMedia)
			{
				MediaFoundationResetVariablesAll();
				return false;
			}

			//Begin media write
			hResult = vMediaFoundationInstance.imfSinkWriter->BeginWriting();
			if (FAILED(hResult))
			{
				MediaFoundationResetVariablesAll();
				return false;
			}

			//Update media loop status
			vMediaFoundationInstance.vMediaWriteLoopAllowed = true;
			vMediaFoundationInstance.vMediaWriteLoopFinishedScreen = false;
			vMediaFoundationInstance.vMediaWriteLoopFinishedAudio = false;

			//Set media loop start time
			LARGE_INTEGER qpcTimeCurrent;
			QueryPerformanceCounter(&qpcTimeCurrent);
			vMediaFoundationInstance.vMediaTimeStartLoop = qpcTimeCurrent.QuadPart;

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
			MediaFoundationResetVariablesAll();
			return false;
		}
	}
}