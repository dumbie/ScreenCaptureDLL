#pragma once
#include "CaptureVariables.h"
#include "CaptureScreen.cpp"
#include "CaptureAudio.cpp"
#include "CaptureVideoStop.cpp"
#include "CaptureWrite.cpp"

namespace
{
	VOID LoopCaptureStatus()
	{
		try
		{
			//Loop variables
			HWND vPreviousForegroundWindow = NULL;
			LONG vPreviousForegroundStyle = NULL;

			//Check capture status while allowed
			while (vCaptureInstance.vCaptureStatusLoopAllowed)
			{
				//Get foreground window
				HWND currentForegroundWindow = GetForegroundWindow();
				LONG currentForegroundStyle = GetWindowLongA(currentForegroundWindow, GWL_STYLE);

				//Check foreground window change
				if (vPreviousForegroundStyle != currentForegroundStyle || vPreviousForegroundWindow != currentForegroundWindow)
				{
					if (!vCaptureSettings.DrawBorder)
					{
						std::cout << "Foreground window / style has changed." << std::endl;
						vWgcInstance.vGraphicsCaptureSession.IsBorderRequired(true);
						vWgcInstance.vGraphicsCaptureSession.IsBorderRequired(false);
					}
				}

				//Update previous variables
				vPreviousForegroundStyle = currentForegroundStyle;
				vPreviousForegroundWindow = currentForegroundWindow;

				//Check capture device change
				if (!vDirectXInstance.iDxgiFactory7->IsCurrent())
				{
					//Trigger capture event
					if (vCaptureEventDeviceChangeDetected)
					{
						std::thread threadEvent(vCaptureEventDeviceChangeDetected);
						threadEvent.detach();
					}

					//Note: triggers on resolution change, hdr switch and driver resets.
					std::cout << "Capture device has changed, reset recommended." << std::endl;
				}

				//Wait for foreground change
				Sleep(1000);
			}
		}
		catch (...) {}

		//Reset thread variables
		vCaptureInstance.vCaptureStatusLoopFinished = true;
	}

	VOID LoopWriteScreen()
	{
		try
		{
			//Get media frame duration time
			ULONGLONG mediaTimeDuration;
			MFFrameRateToAverageTimePerFrame(vMediaSettings.VideoFrameRate, 1, &mediaTimeDuration);

			//Set thread characteristics
			DWORD taskIndex = 1;
			HANDLE mmThread = AvSetMmThreadCharacteristicsW(L"Pro Audio", &taskIndex);
			if (mmThread)
			{
				AvSetMmThreadPriority(mmThread, AVRT_PRIORITY_CRITICAL);
			}

			//Write media in loop while allowed
			while (vMediaFoundationInstance.vMediaWriteLoopAllowed)
			{
				//Get media frame start time
				LARGE_INTEGER qpcTimeCurrent;
				QueryPerformanceCounter(&qpcTimeCurrent);
				ULONGLONG mediaTimeStart = qpcTimeCurrent.QuadPart - vMediaFoundationInstance.vMediaTimeStartLoop;

				//Update screen texture
				if (UpdateScreenTexture())
				{
					//Write media bytes to sink
					std::thread threadWriteSample(WriteMediaTexture2D, vCaptureInstance.iD3D11Texture2D0RenderTargetView, vCaptureDetails.TotalByteSize, false, vMediaFoundationInstance.vOutVideoStreamIndex, mediaTimeStart, mediaTimeDuration);
					threadWriteSample.detach();
				}

				//Delay screen capture
				Sleep(mediaTimeDuration / vReferenceTimeToMilliseconds);
			}
		}
		catch (...) {}

		//Reset thread variables
		vMediaFoundationInstance.vMediaWriteLoopFinishedScreen = true;
	}

	VOID LoopWriteAudio()
	{
		try
		{
			//Get media frame duration time
			ULONGLONG mediaTimeDuration = vReferenceTimeFrameDuration;

			//Set thread characteristics
			DWORD taskIndex = 0;
			HANDLE mmThread = AvSetMmThreadCharacteristicsW(L"Pro Audio", &taskIndex);
			if (mmThread)
			{
				AvSetMmThreadPriority(mmThread, AVRT_PRIORITY_CRITICAL);
			}

			//Write media in loop while allowed
			while (vMediaFoundationInstance.vMediaWriteLoopAllowed)
			{
				//Get media frame start time
				LARGE_INTEGER qpcTimeCurrent;
				QueryPerformanceCounter(&qpcTimeCurrent);
				ULONGLONG mediaTimeStart = qpcTimeCurrent.QuadPart - vMediaFoundationInstance.vMediaTimeStartLoop;

				//Get audio bytes
				std::vector<BYTE> audioBytes = GetAudioBytes();

				//Check audio bytes
				if (!audioBytes.empty())
				{
					//Write media bytes to sink
					WriteMediaDataBytes(audioBytes, true, false, vMediaFoundationInstance.vOutAudioStreamIndex, mediaTimeStart, mediaTimeDuration);
				}
				else
				{
					//Delay audio capture
					//std::cout << "Empty media audio, delaying capture." << std::endl;
					Sleep(1);
				}
			}
		}
		catch (...) {}

		//Reset thread variables
		vMediaFoundationInstance.vMediaWriteLoopFinishedAudio = true;
	}
}