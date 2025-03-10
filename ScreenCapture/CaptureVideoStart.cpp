#pragma once
#include "CaptureVariables.h"
#include "CaptureInitMedia.cpp"
#include "CaptureLoop.cpp"
#include "CaptureReset.cpp"
#include "PlayAudio.cpp"

namespace
{
	CaptureResult CaptureVideoStartCode(const WCHAR* filePath, MediaSettings mediaSettings)
	{
		try
		{
			//Check capture status
			if (vMediaFoundationInstance.vMediaCapturing)
			{
				return { .Status = CaptureStatus::Busy, .Message = SysAllocString(L"Video is already capturing") };
			}

			//Update capture status
			vMediaFoundationInstance.vMediaCapturing = true;

			//Update media settings
			vMediaSettings = mediaSettings;

			//Play audio effect
			PlayAudio(L"Assets\\Capture\\CaptureVideoStart.mp3");

			//Initialize DXGI device manager
			capResult = InitializeDxgiDeviceManager();
			if (capResult.Status != CaptureStatus::Success)
			{
				//Reset media foundation variables
				MediaFoundationResetVariablesAll();

				//Play audio effect
				PlayAudio(L"Assets\\Capture\\CaptureFailed.mp3");

				//Return result
				return capResult;
			}

			//Initialize media foundation
			capResult = InitializeMediaFoundation(filePath);
			if (capResult.Status != CaptureStatus::Success)
			{
				//Reset media foundation variables
				MediaFoundationResetVariablesAll();

				//Play audio effect
				PlayAudio(L"Assets\\Capture\\CaptureFailed.mp3");

				//Return result
				return capResult;
			}

			//Begin media write
			hResult = vMediaFoundationInstance.imfSinkWriter->BeginWriting();
			if (FAILED(hResult))
			{
				//Reset media foundation variables
				MediaFoundationResetVariablesAll();

				//Play audio effect
				PlayAudio(L"Assets\\Capture\\CaptureFailed.mp3");

				//Return result
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"Failed to begin writing media") };
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

			//Return result
			return { .Status = CaptureStatus::Success };
		}
		catch (...)
		{
			//Reset media foundation variables
			MediaFoundationResetVariablesAll();

			//Play audio effect
			PlayAudio(L"Assets\\Capture\\CaptureFailed.mp3");

			//Return result
			return { .Status = CaptureStatus::Failed, .Message = SysAllocString(L"CaptureVideoStartCode failed") };
		}
	}
}