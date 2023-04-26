#pragma once
#include "CaptureVariables.h"

namespace
{
	BYTE* GetAudioBytes(UINT32& audioBytesSize)
	{
		try
		{
			//Get audio buffer
			DWORD mediaFlags;
			UINT32 mediaFramesRead;
			BYTE* mediaBuffer;
			UINT64 devicePosition;
			UINT64 qpcPosition;
			hResult = iAudioCaptureClient->GetBuffer(&mediaBuffer, &mediaFramesRead, &mediaFlags, &devicePosition, &qpcPosition);
			if (FAILED(hResult))
			{
				return NULL;
			}

			//Release audio buffer
			hResult = iAudioCaptureClient->ReleaseBuffer(mediaFramesRead);
			if (FAILED(hResult))
			{
				return NULL;
			}

			//Check if audio needs to be silenced
			BOOL silenceAudioBuffer = false;
			if (mediaFlags & AUDCLNT_BUFFERFLAGS_SILENT)
			{
				//std::cout << "AUDCLNT_BUFFERFLAGS_SILENT" << std::endl;
				silenceAudioBuffer = true;
				vAudioIsMuted = true;
			}
			else if (vAudioIsMuted && mediaFlags & AUDCLNT_BUFFERFLAGS_TIMESTAMP_ERROR)
			{
				//std::cout << "AUDCLNT_BUFFERFLAGS_TIMESTAMP_ERROR" << std::endl;
				silenceAudioBuffer = true;
			}
			else if (vAudioIsMuted && mediaFlags & AUDCLNT_BUFFERFLAGS_DATA_DISCONTINUITY)
			{
				//std::cout << "AUDCLNT_BUFFERFLAGS_DATA_DISCONTINUITY" << std::endl;
				silenceAudioBuffer = true;
			}
			else if (vAudioIsMuted && mediaFramesRead == 0)
			{
				//std::cout << "AUDCLNT_BUFFERFLAGS_NO_FRAMES_READ" << std::endl;
				silenceAudioBuffer = true;
			}

			//Calculate target frames read
			UINT32 mediaFramesTarget = iAudioWaveFormatEx->Format.nSamplesPerSec / 100;

			//Create bytes array
			BYTE* audioBytes = NULL;

			//Check media frames read
			if (!silenceAudioBuffer && mediaFramesRead == mediaFramesTarget)
			{
				//std::cout << "Writing read audio bytes: " << mediaFramesRead << "/" << devicePosition << "/" << qpcPosition << std::endl;

				//Calculate media size
				audioBytesSize = mediaFramesRead * iAudioWaveFormatEx->Format.nBlockAlign;

				//Resize bytes cache
				audioBytes = new BYTE[audioBytesSize];

				//Copy buffer to bytes cache
				memcpy(audioBytes, mediaBuffer, audioBytesSize);

				//Update muted variable
				vAudioIsMuted = false;
			}
			else if (vAudioIsMuted || silenceAudioBuffer)
			{
				//std::cout << "Writing silenced audio bytes: " << mediaFramesRead << "/" << devicePosition << "/" << qpcPosition << std::endl;

				//Calculate media size
				UINT32 audioBytesSize = mediaFramesTarget * iAudioWaveFormatEx->Format.nBlockAlign;

				//Resize bytes cache
				audioBytes = new BYTE[audioBytesSize];

				//Fill bytes cache with silence
				memset(audioBytes, 0, audioBytesSize);

				//Update muted variable
				vAudioIsMuted = true;
			}
			else
			{
				//std::cout << "Not writing audio bytes: " << mediaFramesRead << "/" << devicePosition << "/" << qpcPosition << std::endl;
			}

			//Return result
			return audioBytes;
		}
		catch (...)
		{
			std::cout << "UpdateAudioBytesCache failed." << std::endl;
			return NULL;
		}
	}

	BOOL SetAudioDevice()
	{
		try
		{
			//Create device enumerator
			CComPtr<IMMDeviceEnumerator> deviceEnumerator;
			hResult = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (LPVOID*)&deviceEnumerator);
			if (FAILED(hResult))
			{
				return false;
			}

			//Get default audio device
			hResult = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &iDevice);
			if (FAILED(hResult))
			{
				return false;
			}

			//Activate default audio device
			hResult = iDevice->Activate(__uuidof(IAudioClient3), CLSCTX_ALL, NULL, (LPVOID*)&iAudioClient);
			if (FAILED(hResult))
			{
				return false;
			}

			//Get audio wave format information
			hResult = iAudioClient->GetMixFormat((WAVEFORMATEX**)&iAudioWaveFormatEx);
			if (FAILED(hResult))
			{
				return false;
			}

			//Set audio wave format information
			iAudioWaveFormatEx->SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
			iAudioWaveFormatEx->Format.wBitsPerSample = vMediaSettings.AudioBits;
			iAudioWaveFormatEx->Format.nSamplesPerSec = vMediaSettings.AudioFrequency;
			iAudioWaveFormatEx->Format.nChannels = vMediaSettings.AudioChannels;
			iAudioWaveFormatEx->Format.nBlockAlign = iAudioWaveFormatEx->Format.nChannels * iAudioWaveFormatEx->Format.wBitsPerSample / 8;
			iAudioWaveFormatEx->Format.nAvgBytesPerSec = iAudioWaveFormatEx->Format.nSamplesPerSec * iAudioWaveFormatEx->Format.nBlockAlign;
			iAudioWaveFormatEx->Samples.wValidBitsPerSample = iAudioWaveFormatEx->Format.wBitsPerSample;

			//Initialize default audio device
			UINT initPeriodicity = 0;
			UINT initBufferDuration = vReferenceTimeFrameDuration * 4;
			DWORD initFlags = AUDCLNT_STREAMFLAGS_LOOPBACK | AUDCLNT_STREAMFLAGS_NOPERSIST | AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM | AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY;
			hResult = iAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, initFlags, initBufferDuration, initPeriodicity, (WAVEFORMATEX*)iAudioWaveFormatEx.m_pData, 0);
			if (FAILED(hResult))
			{
				return false;
			}

			//Get audio device service
			hResult = iAudioClient->GetService(IID_PPV_ARGS(&iAudioCaptureClient));
			if (FAILED(hResult))
			{
				return false;
			}

			//Start audio device
			hResult = iAudioClient->Start();
			if (FAILED(hResult))
			{
				return false;
			}

			//Return result
			std::cout << "Audio device started." << std::endl;
			return true;
		}
		catch (...)
		{
			std::cout << "SetAudioDevice failed." << std::endl;
			return false;
		}
	}

	BOOL SetAudioMediaType()
	{
		try
		{
			//Create audio out media type
			CComPtr<IMFMediaType> imfMediaTypeAudioOut;
			hResult = MFCreateMediaType(&imfMediaTypeAudioOut);
			if (FAILED(hResult))
			{
				std::cout << "MFCreateMediaType audio out failed." << std::endl;
				return false;
			}

			//MP3
			if (vMediaSettings.AudioFormat == MFAudioFormat_MP3)
			{
				imfMediaTypeAudioOut->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_MP3);
			}

			//AAC
			if (vMediaSettings.AudioFormat == MFAudioFormat_AAC)
			{
				imfMediaTypeAudioOut->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_AAC);
				imfMediaTypeAudioOut->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, iAudioWaveFormatEx->Format.wBitsPerSample);
			}

			//FLAC
			if (vMediaSettings.AudioFormat == MFAudioFormat_FLAC)
			{
				imfMediaTypeAudioOut->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_FLAC);
				imfMediaTypeAudioOut->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, iAudioWaveFormatEx->Format.wBitsPerSample);
			}

			imfMediaTypeAudioOut->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
			imfMediaTypeAudioOut->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, iAudioWaveFormatEx->Format.nChannels);
			imfMediaTypeAudioOut->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, iAudioWaveFormatEx->Format.nSamplesPerSec);
			imfMediaTypeAudioOut->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, vMediaSettings.AudioBitRate * 1000 / 8);
			hResult = imfSinkWriter->AddStream(imfMediaTypeAudioOut, &vOutAudioStreamIndex);
			if (FAILED(hResult))
			{
				std::cout << "AddStream audio failed." << std::endl;
				return false;
			}

			//Create audio in media type
			CComPtr<IMFMediaType> imfMediaTypeAudioIn;
			hResult = MFCreateMediaType(&imfMediaTypeAudioIn);
			if (FAILED(hResult))
			{
				std::cout << "MFCreateMediaType audio in failed." << std::endl;
				return false;
			}

			imfMediaTypeAudioIn->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
			imfMediaTypeAudioIn->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
			imfMediaTypeAudioIn->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, iAudioWaveFormatEx->Format.wBitsPerSample);
			imfMediaTypeAudioIn->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, iAudioWaveFormatEx->Format.nChannels);
			imfMediaTypeAudioIn->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, iAudioWaveFormatEx->Format.nSamplesPerSec);
			imfMediaTypeAudioIn->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, iAudioWaveFormatEx->Format.nAvgBytesPerSec);
			imfMediaTypeAudioIn->SetUINT32(MF_MT_AUDIO_BLOCK_ALIGNMENT, iAudioWaveFormatEx->Format.nBlockAlign);
			hResult = imfSinkWriter->SetInputMediaType(vOutAudioStreamIndex, imfMediaTypeAudioIn, NULL);
			if (FAILED(hResult))
			{
				std::cout << "SetInputMediaType audio failed." << std::endl;
				return false;
			}

			return true;
		}
		catch (...)
		{
			std::cout << "SetAudioMediaType failed." << std::endl;
			return false;
		}
	}
}