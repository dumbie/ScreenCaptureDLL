#pragma once
#include "CaptureIncludes.h"
#include "CaptureVariables.h"
#include "CaptureScreen.cpp"
#include "CaptureVideo.cpp"
#include "CaptureAudio.cpp"
#include "CaptureWrite.cpp"

namespace
{
	VOID LoopWriteScreen(UINT captureInstanceId)
	{
		try
		{
			//Get media frame duration time
			ULONGLONG mediaTimeDuration;
			MFFrameRateToAverageTimePerFrame(vCaptureInstances[captureInstanceId].vMediaSettings.VideoFrameRate, 1, &mediaTimeDuration);

			//Set thread characteristics
			DWORD taskIndex = 1;
			HANDLE mmThread = AvSetMmThreadCharacteristicsW(L"Pro Audio", &taskIndex);
			if (mmThread)
			{
				AvSetMmThreadPriority(mmThread, AVRT_PRIORITY_CRITICAL);
			}

			//Write media in loop while allowed
			while (vCaptureInstances[captureInstanceId].vMediaWriteLoopAllowed)
			{
				//Get media frame start time
				LARGE_INTEGER qpcTimeCurrent;
				QueryPerformanceCounter(&qpcTimeCurrent);
				ULONGLONG mediaTimeStart = qpcTimeCurrent.QuadPart - vCaptureInstances[captureInstanceId].vMediaTimeStartLoop;

				//Update screen texture
				if (UpdateScreenTexture(captureInstanceId, true))
				{
					//Write media bytes to sink
					std::thread threadWriteSample(WriteMediaTexture2D, captureInstanceId, vCaptureInstances[captureInstanceId].iD3D11Texture2D0RenderTargetView, vCaptureInstances[captureInstanceId].vCaptureDetails.TotalByteSize, true, vCaptureInstances[captureInstanceId].vOutVideoStreamIndex, mediaTimeStart, mediaTimeDuration);
					threadWriteSample.detach();
				}

				//Delay screen capture
				Sleep(mediaTimeDuration / vReferenceTimeToMilliseconds);
			}
		}
		catch (...) {}

		//Reset thread variables
		vCaptureInstances[captureInstanceId].vMediaWriteLoopFinishedScreen = true;
	}

	VOID LoopWriteAudio(UINT captureInstanceId)
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
			while (vCaptureInstances[captureInstanceId].vMediaWriteLoopAllowed)
			{
				//Get media frame start time
				LARGE_INTEGER qpcTimeCurrent;
				QueryPerformanceCounter(&qpcTimeCurrent);
				ULONGLONG mediaTimeStart = qpcTimeCurrent.QuadPart - vCaptureInstances[captureInstanceId].vMediaTimeStartLoop;

				//Get audio bytes
				std::vector<BYTE> audioBytes = GetAudioBytes(captureInstanceId);

				//Check audio bytes
				if (!audioBytes.empty())
				{
					//Write media bytes to sink
					WriteMediaDataBytes(captureInstanceId, audioBytes, true, false, vCaptureInstances[captureInstanceId].vOutAudioStreamIndex, mediaTimeStart, mediaTimeDuration);
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
		vCaptureInstances[captureInstanceId].vMediaWriteLoopFinishedAudio = true;
	}
}