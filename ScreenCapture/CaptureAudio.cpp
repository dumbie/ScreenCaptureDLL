#pragma once
#include "CaptureVariables.h"

namespace
{
	MediaFrameBytes CaptureAudioBytes()
	{
		MediaFrameBytes mediaFrameBytes{};
		try
		{
			//Get audio buffer
			DWORD bufferFlags;
			UINT32 numFramesRead;
			hResult = iAudioCaptureClient->GetBuffer(&mediaFrameBytes.Bytes, &numFramesRead, &bufferFlags, NULL, NULL);
			if (FAILED(hResult))
			{
				return mediaFrameBytes;
			}

			//Release audio buffer
			hResult = iAudioCaptureClient->ReleaseBuffer(numFramesRead);
			if (FAILED(hResult))
			{
				return mediaFrameBytes;
			}

			//Calculate media size
			UINT32 mediaSize = numFramesRead * iAudioWaveFormatEx->Format.nBlockAlign;

			//Calculate media duration
			ULONGLONG mediaTimeDuration = numFramesRead;
			mediaTimeDuration *= vReferenceTimeToSeconds;
			mediaTimeDuration /= iAudioWaveFormatEx->Format.nSamplesPerSec;

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
			REFERENCE_TIME initBufferDuration = vReferenceTimeToSeconds;
			UINT32 initFlags = AUDCLNT_STREAMFLAGS_LOOPBACK | AUDCLNT_STREAMFLAGS_NOPERSIST | AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM | AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY;
			hResult = iAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, initFlags, initBufferDuration, 0, (WAVEFORMATEX*)iAudioWaveFormatEx.m_pData, 0);
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
			imfMediaTypeAudioIn->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, iAudioWaveFormatEx->Format.wBitsPerSample);
			imfMediaTypeAudioIn->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, iAudioWaveFormatEx->Format.nChannels);
			imfMediaTypeAudioIn->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, iAudioWaveFormatEx->Format.nSamplesPerSec);
			imfMediaTypeAudioIn->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, iAudioWaveFormatEx->Format.nAvgBytesPerSec);
			imfMediaTypeAudioIn->SetUINT32(MF_MT_AUDIO_BLOCK_ALIGNMENT, iAudioWaveFormatEx->Format.nBlockAlign);
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