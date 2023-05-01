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

				//Get screen bytes
				SafeBytes screenBytes = GetScreenBytes(false, true);

				//Update screen bytes
				if (!screenBytes.IsEmpty())
				{
					//Release screen bytes cache
					vScreenBytesCacheVideo.Release();

					//Update screen bytes cache
					vScreenBytesCacheVideo = screenBytes;
				}

				//Check screen bytes
				if (!vScreenBytesCacheVideo.IsEmpty())
				{
					//Write media bytes to sink
					ULONGLONG mediaTimeStart = qpcTimeCurrent.QuadPart - vMediaTimeStartLoop;
					WriteMediaDataBytes(vScreenBytesCacheVideo, false, vOutVideoStreamIndex, mediaTimeStart, mediaTimeDuration);
				}
				else
				{
					//std::cout << "Empty media screen, delaying capture." << std::endl;

					//Delay screen capture
					Sleep(1 / vReferenceTimeToMilliseconds);
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

				//Get audio bytes
				SafeBytes audioBytes = GetAudioBytes();

				//Check audio bytes
				if (!audioBytes.IsEmpty())
				{
					//Write media bytes to sink
					ULONGLONG mediaTimeStart = qpcTimeCurrent.QuadPart - vMediaTimeStartLoop;
					WriteMediaDataBytes(audioBytes, true, vOutAudioStreamIndex, mediaTimeStart, mediaTimeDuration);
				}
				else
				{
					//std::cout << "Empty media audio, delaying capture." << std::endl;

					//Delay audio capture
					Sleep(1 / vReferenceTimeToMilliseconds);
				}
			}
		}
		catch (...) {}

		//Reset thread variables
		vMediaWriteLoopFinishedAudio = true;
	}
}