#pragma once
#include "CaptureVariables.h"

namespace
{
	CaptureDataAudio GetAudioData()
	{
		CaptureDataAudio dataAudio{};
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
				return dataAudio;
			}

			//Release audio buffer
			hResult = vMediaFoundationInstance.iAudioClientCapture->ReleaseBuffer(mediaFramesRead);
			if (FAILED(hResult))
			{
				return dataAudio;
			}

			//Check if audio needs to be silenced
			if (mediaFlags & AUDCLNT_BUFFERFLAGS_SILENT)
			{
				//AVDebugWriteLine("AUDCLNT_BUFFERFLAGS_SILENT");
				dataAudio.Silent = true;
			}

			//Check for audio data discontinuity
			if (mediaFlags & AUDCLNT_BUFFERFLAGS_DATA_DISCONTINUITY)
			{
				//AVDebugWriteLine("AUDCLNT_BUFFERFLAGS_DATA_DISCONTINUITY");
				dataAudio.DataDiscontinuity = true;
			}

			//Check for audio timestamp error
			if (mediaFlags & AUDCLNT_BUFFERFLAGS_TIMESTAMP_ERROR)
			{
				//AVDebugWriteLine("AUDCLNT_BUFFERFLAGS_TIMESTAMP_ERROR");
				dataAudio.TimestampError = true;
			}

			//Calculate target frames read
			UINT32 mediaFramesTarget = vMediaFoundationInstance.iAudioWaveFormatExCapture.Format.nSamplesPerSec / 100;

			//Check media frames read
			if (mediaFramesRead >= mediaFramesTarget)
			{
				//Calculate media size
				UINT audioBytesSize = mediaFramesRead * vMediaFoundationInstance.iAudioWaveFormatExCapture.Format.nBlockAlign;

				//Resize bytes array
				dataAudio.AudioBytes.resize(audioBytesSize);

				//Copy buffer to bytes cache
				memcpy(dataAudio.AudioBytes.data(), mediaBuffer, audioBytesSize);

				//Return result
				//AVDebugWriteLine("Writing read audio bytes: " << mediaFramesRead << "/" << devicePosition << "/" << qpcPosition << "/" << mediaFlags);
				return dataAudio;
			}
			else if (dataAudio.Silent)
			{
				//Calculate media size
				UINT audioBytesSize = mediaFramesRead * vMediaFoundationInstance.iAudioWaveFormatExCapture.Format.nBlockAlign;

				//Resize bytes array
				dataAudio.AudioBytes.resize(audioBytesSize);

				//Fill bytes cache with silence
				memset(dataAudio.AudioBytes.data(), 0, audioBytesSize);

				//Return result
				//AVDebugWriteLine("Writing silenced audio bytes: " << mediaFramesRead << "/" << devicePosition << "/" << qpcPosition << "/" << mediaFlags);
				return dataAudio;
			}
			else
			{
				//Return result
				//AVDebugWriteLine("No audio bytes read.");
				return dataAudio;
			}
		}
		catch (...)
		{
			//Return result
			AVDebugWriteLine("GetAudioData failed.");
			return dataAudio;
		}
	}
}