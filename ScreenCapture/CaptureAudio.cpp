#pragma once
#include "CaptureVariables.h"

namespace
{
	std::vector<BYTE> GetAudioBytes()
	{
		try
		{
			//Get audio buffer
			DWORD mediaFlags;
			UINT32 mediaFramesRead;
			BYTE* mediaBuffer;
			UINT64 devicePosition;
			UINT64 qpcPosition;
			hResult = vCaptureInstance.iAudioClientCapture->GetBuffer(&mediaBuffer, &mediaFramesRead, &mediaFlags, &devicePosition, &qpcPosition);
			if (FAILED(hResult))
			{
				return {};
			}

			//Release audio buffer
			hResult = vCaptureInstance.iAudioClientCapture->ReleaseBuffer(mediaFramesRead);
			if (FAILED(hResult))
			{
				return {};
			}

			//Check if audio needs to be silenced
			BOOL silenceAudioBuffer = false;
			if (mediaFlags & AUDCLNT_BUFFERFLAGS_SILENT)
			{
				//std::cout << "AUDCLNT_BUFFERFLAGS_SILENT" << std::endl;
				silenceAudioBuffer = true;
			}

			//Calculate target frames read
			UINT32 mediaFramesTarget = vCaptureInstance.iAudioWaveFormatExCapture->Format.nSamplesPerSec / 100;

			//Check media frames read
			if (mediaFramesRead == mediaFramesTarget)
			{
				//std::cout << "Writing read audio bytes: " << mediaFramesRead << "/" << devicePosition << "/" << qpcPosition << "/" << mediaFlags << std::endl;

				//Calculate media size
				UINT audioBytesSize = mediaFramesRead * vCaptureInstance.iAudioWaveFormatExCapture->Format.nBlockAlign;

				//Create bytes array
				std::vector<BYTE> audioBytes(audioBytesSize);

				//Copy buffer to bytes cache
				memcpy(audioBytes.data(), mediaBuffer, audioBytesSize);

				//Return result
				return audioBytes;
			}
			else if (silenceAudioBuffer)
			{
				//std::cout << "Writing silenced audio bytes: " << mediaFramesRead << "/" << devicePosition << "/" << qpcPosition << "/" << mediaFlags << std::endl;

				//Calculate media size
				UINT audioBytesSize = mediaFramesTarget * vCaptureInstance.iAudioWaveFormatExCapture->Format.nBlockAlign;

				//Create bytes array
				std::vector<BYTE> audioBytes(audioBytesSize);

				//Fill bytes cache with silence
				memset(audioBytes.data(), 0, audioBytesSize);

				//Return result
				return audioBytes;
			}
			else
			{
				//std::cout << "No audio bytes read." << std::endl;
				return {};
			}
		}
		catch (...)
		{
			std::cout << "GetAudioBytes failed." << std::endl;
			return {};
		}
	}
}