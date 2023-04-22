#pragma once
#include "CaptureIncludes.h"
#include "CaptureVariables.h"
#include "CaptureScreen.cpp"
#include "CaptureVideo.cpp"
#include "CaptureAudio.cpp"
#include "CaptureMedia.cpp"

namespace
{
	VOID LoopWriteMedia()
	{
		try
		{
			//Set thread variables
			vMediaTimeNext = 0;
			vMediaTimeStart = 0;
			vMediaTimeDuration = 0;

			while (vMediaWriteLoopAllowed)
			{
				//Check next write time
				LARGE_INTEGER qpcTimeStart;
				QueryPerformanceCounter(&qpcTimeStart);
				LONGLONG differenceTime = qpcTimeStart.QuadPart - vMediaTimeNext;
				if (differenceTime < vReferenceTimeFrameDuration)
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

				//Update audio bytes and check
				UpdateAudioBytesCache();
				if (vAudioBytesCache.empty())
				{
					continue;
				}

				WriteMediaDataBytes(imfSinkWriter, vScreenBytesCache.data(), vScreenBytesCache.size(), vOutVideoStreamIndex, vMediaTimeStart, vMediaTimeDuration);
				WriteMediaDataBytes(imfSinkWriter, vAudioBytesCache.data(), vAudioBytesCache.size(), vOutAudioStreamIndex, vMediaTimeStart, vMediaTimeDuration);
				std::cout << "Written media at: " << (vMediaTimeStart / vReferenceTimeToSeconds) << "s/" << vMediaTimeStart << " duration: " << vMediaTimeDuration << " last: " << vMediaTimeNext << std::endl;
				//std::cout << "Written screen size: " << vScreenBytesCache.size() << std::endl;
				//std::cout << "Written audio size: " << vAudioBytesCache.size() << std::endl;

				//Update media time start
				vMediaTimeStart += vMediaTimeDuration;

				//Update media time next
				vMediaTimeNext = qpcTimeStart.QuadPart;
			}
		}
		catch (...) {}

		//Reset thread variables
		vMediaWriteLoopFinished = true;
	}
}