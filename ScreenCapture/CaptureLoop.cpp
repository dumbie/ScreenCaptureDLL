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

				//Check next media write time
				ULONGLONG differenceTime = qpcTimeCurrent.QuadPart - vMediaTimeNextScreen;
				if (differenceTime < mediaTimeDuration)
				{
					//std::cout << "Delaying media write, writing to fast: " << differenceTime << std::endl;
					continue;
				}

				//Update screen bytes and check
				UpdateScreenBytesCache(false, true);
				if (vScreenBytesCache.empty())
				{
					continue;
				}

				//Write media bytes to sink
				ULONGLONG mediaTimeStart = qpcTimeCurrent.QuadPart - vMediaTimeStartLoop;
				WriteMediaDataBytes(imfSinkWriter, vScreenBytesCache.data(), vScreenBytesCache.size(), vOutVideoStreamIndex, mediaTimeStart, mediaTimeDuration);
				//std::cout << "Written media screen at: " << (mediaTimeStart / vReferenceTimeToSeconds) << "s/" << mediaTimeStart << " duration: " << mediaTimeDuration << " size: " << vScreenBytesCache.size() << std::endl;

				//Update media next time
				vMediaTimeNextScreen = qpcTimeCurrent.QuadPart;
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

				//Check next media write time
				ULONGLONG differenceTime = qpcTimeCurrent.QuadPart - vMediaTimeNextAudio;
				if (differenceTime < mediaTimeDuration)
				{
					//std::cout << "Delaying media write, writing to fast: " << differenceTime << std::endl;
					continue;
				}

				//Update audio bytes and check
				UpdateAudioBytesCache();
				if (vAudioBytesCache.empty())
				{
					continue;
				}

				//Write media bytes to sink
				ULONGLONG mediaTimeStart = qpcTimeCurrent.QuadPart - vMediaTimeStartLoop;
				WriteMediaDataBytes(imfSinkWriter, vAudioBytesCache.data(), vAudioBytesCache.size(), vOutAudioStreamIndex, mediaTimeStart, mediaTimeDuration);
				//std::cout << "Written media audio at: " << (mediaTimeStart / vReferenceTimeToSeconds) << "s/" << mediaTimeStart << " duration: " << mediaTimeDuration << " size: " << vAudioBytesCache.size() << std::endl;

				//Update media next time
				vMediaTimeNextAudio = qpcTimeCurrent.QuadPart;
			}
		}
		catch (...) {}

		//Reset thread variables
		vMediaWriteLoopFinishedAudio = true;
	}
}