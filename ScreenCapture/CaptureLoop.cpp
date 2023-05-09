#pragma once
#include "CaptureIncludes.h"
#include "CaptureVariables.h"
#include "CaptureScreen.cpp"
#include "CaptureVideo.cpp"
#include "CaptureAudio.cpp"
#include "CaptureMedia.cpp"

namespace
{
	VOID LoopWriteScreen()
	{
		try
		{
			//Get media frame duration time
			ULONGLONG mediaTimeDuration;
			MFFrameRateToAverageTimePerFrame(vMediaSettings.VideoFrameRate, 1, &mediaTimeDuration);

			//Set thread characteristics
			DWORD taskIndex = 0;
			HANDLE mmThread = AvSetMmThreadCharacteristicsW(L"Pro Audio", &taskIndex);
			if (mmThread)
			{
				AvSetMmThreadPriority(mmThread, AVRT_PRIORITY_CRITICAL);
			}

			//Write media in loop while allowed
			while (vMediaWriteLoopAllowed)
			{
				//Get media frame start time
				LARGE_INTEGER qpcTimeCurrent;
				QueryPerformanceCounter(&qpcTimeCurrent);
				ULONGLONG mediaTimeStart = qpcTimeCurrent.QuadPart - vMediaTimeStartLoop;

				//Get screen bytes
				SafeBytes screenBytes = GetScreenBytes(true, false);

				//Update screen bytes
				if (!screenBytes.IsEmpty())
				{
					//Write media bytes to sink
					std::thread threadWriteSample(WriteMediaDataBytes, screenBytes, true, true, vOutVideoStreamIndex, mediaTimeStart, mediaTimeDuration);
					threadWriteSample.detach();
				}
				else
				{
					//Delay screen capture
					//std::cout << "Empty media screen, delaying capture." << std::endl;
					Sleep(1);
				}
			}
		}
		catch (...) {}

		//Reset thread variables
		vMediaWriteLoopFinishedScreen = true;
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
			while (vMediaWriteLoopAllowed)
			{
				//Get media frame start time
				LARGE_INTEGER qpcTimeCurrent;
				QueryPerformanceCounter(&qpcTimeCurrent);
				ULONGLONG mediaTimeStart = qpcTimeCurrent.QuadPart - vMediaTimeStartLoop;

				//Get audio bytes
				SafeBytes audioBytes = GetAudioBytes();

				//Check audio bytes
				if (!audioBytes.IsEmpty())
				{
					//Write media bytes to sink
					WriteMediaDataBytes(audioBytes, true, false, vOutAudioStreamIndex, mediaTimeStart, mediaTimeDuration);
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
		vMediaWriteLoopFinishedAudio = true;
	}
}