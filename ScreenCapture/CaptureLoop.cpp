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
			while (vWgcInstance.vCaptureStatusLoopAllowed)
			{
				//Wait for foreground change
				Sleep(500);

				//Get foreground window
				HWND currentForegroundWindow = GetForegroundWindow();
				LONG currentForegroundStyle = GetWindowLongA(currentForegroundWindow, GWL_STYLE);

				//Fix find way to detect monitor awake
				//Fix find way to detect display driver reset
				//Fix find way to detect display hdr switching

				//Check foreground window change
				if (vPreviousForegroundStyle != currentForegroundStyle || vPreviousForegroundWindow != currentForegroundWindow)
				{
					std::cout << "Foreground window / style has changed." << std::endl;
					vWgcInstance.vGraphicsCaptureSession.IsBorderRequired(true);
					vWgcInstance.vGraphicsCaptureSession.IsBorderRequired(false);
				}

				//Update previous variables
				vPreviousForegroundStyle = currentForegroundStyle;
				vPreviousForegroundWindow = currentForegroundWindow;

				//Check screen resolution change
				if (vWgcInstance.vFrameSizeCurrent != vWgcInstance.vGraphicsCaptureItem.Size())
				{
					std::cout << "Frame resolution has changed." << std::endl;
				}
			}
		}
		catch (...) {}

		//Reset thread variables
		vWgcInstance.vCaptureStatusLoopFinished = true;
	}

	VOID LoopWriteScreen()
	{
		try
		{
			//Get media frame duration time
			ULONGLONG mediaTimeDuration;
			MFFrameRateToAverageTimePerFrame(vCaptureInstance.vMediaSettings.VideoFrameRate, 1, &mediaTimeDuration);

			//Set thread characteristics
			DWORD taskIndex = 1;
			HANDLE mmThread = AvSetMmThreadCharacteristicsW(L"Pro Audio", &taskIndex);
			if (mmThread)
			{
				AvSetMmThreadPriority(mmThread, AVRT_PRIORITY_CRITICAL);
			}

			//Write media in loop while allowed
			while (vCaptureInstance.vMediaWriteLoopAllowed)
			{
				//Get media frame start time
				LARGE_INTEGER qpcTimeCurrent;
				QueryPerformanceCounter(&qpcTimeCurrent);
				ULONGLONG mediaTimeStart = qpcTimeCurrent.QuadPart - vCaptureInstance.vMediaTimeStartLoop;

				//Update screen texture
				if (UpdateScreenTexture())
				{
					//Write media bytes to sink
					std::thread threadWriteSample(WriteMediaTexture2D, vCaptureInstance.iD3D11Texture2D0RenderTargetView, vCaptureDetails.TotalByteSize, false, vCaptureInstance.vOutVideoStreamIndex, mediaTimeStart, mediaTimeDuration);
					threadWriteSample.detach();

					//Reset capture fail count
					vCaptureInstance.vCaptureFailCount = 0;
				}
				else
				{
					//Update capture fail count
					vCaptureInstance.vCaptureFailCount++;
				}

				//Check capture fail count
				if (vCaptureInstance.vCaptureFailCount > 100)
				{
					std::cout << "Screen capture failed multiple times, stopping video capture." << std::endl;
					CaptureVideoStopCode();
				}

				//Delay screen capture
				Sleep(mediaTimeDuration / vReferenceTimeToMilliseconds);
			}
		}
		catch (...) {}

		//Reset thread variables
		vCaptureInstance.vMediaWriteLoopFinishedScreen = true;
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
			while (vCaptureInstance.vMediaWriteLoopAllowed)
			{
				//Get media frame start time
				LARGE_INTEGER qpcTimeCurrent;
				QueryPerformanceCounter(&qpcTimeCurrent);
				ULONGLONG mediaTimeStart = qpcTimeCurrent.QuadPart - vCaptureInstance.vMediaTimeStartLoop;

				//Get audio bytes
				std::vector<BYTE> audioBytes = GetAudioBytes();

				//Check audio bytes
				if (!audioBytes.empty())
				{
					//Write media bytes to sink
					WriteMediaDataBytes(audioBytes, true, false, vCaptureInstance.vOutAudioStreamIndex, mediaTimeStart, mediaTimeDuration);
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
		vCaptureInstance.vMediaWriteLoopFinishedAudio = true;
	}
}