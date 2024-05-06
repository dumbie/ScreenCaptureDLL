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
			hResult = vMediaFoundationInstance.iAudioClientCapture->GetBuffer(&mediaBuffer, &mediaFramesRead, &mediaFlags, &devicePosition, &qpcPosition);
			if (FAILED(hResult))
			{
				return {};
			}

			//Release audio buffer
			hResult = vMediaFoundationInstance.iAudioClientCapture->ReleaseBuffer(mediaFramesRead);
			if (FAILED(hResult))
			{
				return {};
			}

			//Check if audio needs to be silenced
			BOOL silenceAudioBuffer = false;
			if (mediaFlags & AUDCLNT_BUFFERFLAGS_SILENT)
			{
				//AVDebugWriteLine("AUDCLNT_BUFFERFLAGS_SILENT");
				silenceAudioBuffer = true;
			}

			//Calculate target frames read
			UINT32 mediaFramesTarget = vMediaFoundationInstance.iAudioWaveFormatExCapture->Format.nSamplesPerSec / 100;

			//Check media frames read
			if (mediaFramesRead == mediaFramesTarget)
			{
				//AVDebugWriteLine("Writing read audio bytes: " << mediaFramesRead << "/" << devicePosition << "/" << qpcPosition << "/" << mediaFlags);

				//Calculate media size
				UINT audioBytesSize = mediaFramesRead * vMediaFoundationInstance.iAudioWaveFormatExCapture->Format.nBlockAlign;

				//Create bytes array
				std::vector<BYTE> audioBytes(audioBytesSize);

				//Copy buffer to bytes cache
				memcpy(audioBytes.data(), mediaBuffer, audioBytesSize);

				//Return result
				return audioBytes;
			}
			else if (silenceAudioBuffer)
			{
				//AVDebugWriteLine("Writing silenced audio bytes: " << mediaFramesRead << "/" << devicePosition << "/" << qpcPosition << "/" << mediaFlags);

				//Calculate media size
				UINT audioBytesSize = mediaFramesTarget * vMediaFoundationInstance.iAudioWaveFormatExCapture->Format.nBlockAlign;

				//Create bytes array
				std::vector<BYTE> audioBytes(audioBytesSize);

				//Fill bytes cache with silence
				memset(audioBytes.data(), 0, audioBytesSize);

				//Return result
				return audioBytes;
			}
			else
			{
				//AVDebugWriteLine("No audio bytes read.");
				return {};
			}
		}
		catch (...)
		{
			AVDebugWriteLine("GetAudioBytes failed.");
			return {};
		}
	}
}