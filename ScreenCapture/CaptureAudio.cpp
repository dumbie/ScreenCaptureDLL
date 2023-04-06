#pragma once
#include "CaptureVariables.h"

namespace
{
	MediaFrameBytes CaptureAudioBytes()
	{
		MediaFrameBytes mediaFrameBytes{};
		try
		{
			DWORD bufferFlags;
			UINT32 numFramesRead;
			hResult = iAudioCaptureClient->GetBuffer(&mediaFrameBytes.Bytes, &numFramesRead, &bufferFlags, NULL, NULL);
			if (FAILED(hResult))
			{
				return mediaFrameBytes;
			}

			hResult = iAudioCaptureClient->ReleaseBuffer(numFramesRead);
			if (FAILED(hResult))
			{
				return mediaFrameBytes;
			}

			//Calculate media size
			UINT32 mediaSize = numFramesRead * iAudioWaveFormatEx->nBlockAlign;

			//Calculate media duration
			ULONGLONG mediaTimeDuration = numFramesRead;
			mediaTimeDuration *= vReferenceTimeToSeconds;
			mediaTimeDuration /= iAudioWaveFormatEx->nSamplesPerSec;

			//Return media bytes
			mediaFrameBytes.Size = mediaSize;
			mediaFrameBytes.TimeDuration = mediaTimeDuration;
		}
		catch (...) {}
		return mediaFrameBytes;
	}

	BOOL SetAudioDevice(CComPtr<IMFSinkWriterEx> imfSinkWriter)
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
			hResult = iDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (LPVOID*)&iAudioClient);
			if (FAILED(hResult))
			{
				return false;
			}

			//Get audio wave format information
			hResult = iAudioClient->GetMixFormat(&iAudioWaveFormatEx);
			if (FAILED(hResult))
			{
				return false;
			}

			//Set audio wave format information
			WAVEFORMATEXTENSIBLE* iAudioWaveFormatExtensible = (WAVEFORMATEXTENSIBLE*)iAudioWaveFormatEx;
			iAudioWaveFormatExtensible->SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
			iAudioWaveFormatExtensible->Format.wBitsPerSample = vMediaSettings.AudioBits;
			iAudioWaveFormatExtensible->Format.nSamplesPerSec = vMediaSettings.AudioFrequency;
			iAudioWaveFormatExtensible->Format.nChannels = vMediaSettings.AudioChannels;
			iAudioWaveFormatExtensible->Format.nBlockAlign = iAudioWaveFormatExtensible->Format.nChannels * (iAudioWaveFormatExtensible->Format.wBitsPerSample / 8);
			iAudioWaveFormatExtensible->Format.nAvgBytesPerSec = iAudioWaveFormatExtensible->Format.nBlockAlign * iAudioWaveFormatExtensible->Format.nSamplesPerSec;
			iAudioWaveFormatExtensible->Samples.wValidBitsPerSample = iAudioWaveFormatExtensible->Format.wBitsPerSample;

			//Initialize default audio device
			UINT initFlags = AUDCLNT_STREAMFLAGS_LOOPBACK | AUDCLNT_STREAMFLAGS_NOPERSIST | AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM | AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY;
			hResult = iAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, initFlags, vReferenceTimeToSeconds, 0, iAudioWaveFormatEx, 0);
			if (FAILED(hResult))
			{
				return false;
			}

			//Get audio device service
			hResult = iAudioClient->GetService(__uuidof(IAudioCaptureClient), (LPVOID*)&iAudioCaptureClient);
			if (FAILED(hResult))
			{
				return false;
			}

			//Start audio device
			iAudioClient->Start();

			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	BOOL SetAudioMediaType(CComPtr<IMFSinkWriterEx> imfSinkWriter)
	{
		try
		{
			//Create audio out media type
			CComPtr<IMFMediaType> imfMediaTypeAudioOut;
			hResult = MFCreateMediaType(&imfMediaTypeAudioOut);
			if (FAILED(hResult))
			{
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
				imfMediaTypeAudioOut->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, vMediaSettings.AudioBits);
			}

			//FLAC
			if (vMediaSettings.AudioFormat == MFAudioFormat_FLAC)
			{
				imfMediaTypeAudioOut->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_FLAC);
				imfMediaTypeAudioOut->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, vMediaSettings.AudioBits);
			}

			imfMediaTypeAudioOut->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
			imfMediaTypeAudioOut->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, vMediaSettings.AudioChannels);
			imfMediaTypeAudioOut->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, vMediaSettings.AudioFrequency);
			imfMediaTypeAudioOut->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, vMediaSettings.AudioBitRate * 1000 / 8);
			hResult = imfSinkWriter->AddStream(imfMediaTypeAudioOut, &vOutAudioStreamIndex);
			if (FAILED(hResult))
			{
				return false;
			}

			//Create audio in media type
			CComPtr<IMFMediaType> imfMediaTypeAudioIn;
			hResult = MFCreateMediaType(&imfMediaTypeAudioIn);
			if (FAILED(hResult))
			{
				return false;
			}

			imfMediaTypeAudioIn->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
			imfMediaTypeAudioIn->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
			imfMediaTypeAudioIn->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, iAudioWaveFormatEx->wBitsPerSample);
			imfMediaTypeAudioIn->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, iAudioWaveFormatEx->nChannels);
			imfMediaTypeAudioIn->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, iAudioWaveFormatEx->nSamplesPerSec);
			imfMediaTypeAudioIn->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, iAudioWaveFormatEx->nAvgBytesPerSec);
			imfMediaTypeAudioIn->SetUINT32(MF_MT_AUDIO_BLOCK_ALIGNMENT, iAudioWaveFormatEx->nBlockAlign);
			hResult = imfSinkWriter->SetInputMediaType(vOutAudioStreamIndex, imfMediaTypeAudioIn, NULL);
			if (FAILED(hResult))
			{
				return false;
			}

			return true;
		}
		catch (...)
		{
			return false;
		}
	}
}