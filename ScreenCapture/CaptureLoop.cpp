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
				BYTE* screenBytes = GetScreenBytes(true, true);

				//Check screen bytes
				if (screenBytes != NULL)
				{
					//Write media bytes to sink
					ULONGLONG mediaTimeStart = qpcTimeCurrent.QuadPart - vMediaTimeStartLoop;
					WriteMediaDataBytes(screenBytes, vCaptureDetails.TotalByteSize, vOutVideoStreamIndex, mediaTimeStart, mediaTimeDuration);
					std::cout << "Written media screen at: " << (mediaTimeStart / vReferenceTimeToSeconds) << "s/" << mediaTimeStart << " duration: " << mediaTimeDuration << " size: " << vCaptureDetails.TotalByteSize << std::endl;

					//Clear screen bytes
					DeleteDataBytes(screenBytes);
				}
				else
				{
					//Delay screen capture
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

				//Get audio bytes
				UINT32 audioBytesSize;
				BYTE* audioBytes = GetAudioBytes(audioBytesSize);

				//Check audio bytes
				if (audioBytes != NULL)
				{
					//Write media bytes to sink
					ULONGLONG mediaTimeStart = qpcTimeCurrent.QuadPart - vMediaTimeStartLoop;
					WriteMediaDataBytes(audioBytes, audioBytesSize, vOutAudioStreamIndex, mediaTimeStart, mediaTimeDuration);
					std::cout << "Written media audio at: " << (mediaTimeStart / vReferenceTimeToSeconds) << "s/" << mediaTimeStart << " duration: " << mediaTimeDuration << " size: " << audioBytesSize << std::endl;

					//Clear audio bytes
					DeleteDataBytes(audioBytes);
				}
				else
				{
					//Delay audio capture
					Sleep(1);
				}
			}
		}
		catch (...) {}

		//Reset thread variables
		vMediaWriteLoopFinishedAudio = true;
	}
}