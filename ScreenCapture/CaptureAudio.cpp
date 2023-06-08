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
			hResult = iAudioClientCapture->GetBuffer(&mediaBuffer, &mediaFramesRead, &mediaFlags, &devicePosition, &qpcPosition);
			if (FAILED(hResult))
			{
				return {};
			}

			//Release audio buffer
			hResult = iAudioClientCapture->ReleaseBuffer(mediaFramesRead);
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
			UINT32 mediaFramesTarget = iAudioWaveFormatExCapture->Format.nSamplesPerSec / 100;

			//Check media frames read
			if (mediaFramesRead == mediaFramesTarget)
			{
				//std::cout << "Writing read audio bytes: " << mediaFramesRead << "/" << devicePosition << "/" << qpcPosition << "/" << mediaFlags << std::endl;

				//Calculate media size
				UINT audioBytesSize = mediaFramesRead * iAudioWaveFormatExCapture->Format.nBlockAlign;

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
				UINT audioBytesSize = mediaFramesTarget * iAudioWaveFormatExCapture->Format.nBlockAlign;

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

	BOOL SetAudioDeviceRender()
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
			hResult = iDevice->Activate(__uuidof(IAudioClient3), CLSCTX_ALL, NULL, (LPVOID*)&iAudioDeviceRender);
			if (FAILED(hResult))
			{
				return false;
			}

			//Get audio wave format information
			hResult = iAudioDeviceRender->GetMixFormat((WAVEFORMATEX**)&iAudioWaveFormatExRender);
			if (FAILED(hResult))
			{
				return false;
			}

			//Initialize default audio device
			UINT initPeriodicity = 0;
			UINT initBufferDuration = 0;
			DWORD initFlags = 0;
			hResult = iAudioDeviceRender->Initialize(AUDCLNT_SHAREMODE_SHARED, initFlags, initBufferDuration, initPeriodicity, (WAVEFORMATEX*)iAudioWaveFormatExRender.m_pData, 0);
			if (FAILED(hResult))
			{
				return false;
			}

			//Get audio device service
			hResult = iAudioDeviceRender->GetService(IID_PPV_ARGS(&iAudioClientRender));
			if (FAILED(hResult))
			{
				return false;
			}

			//Start audio device
			hResult = iAudioDeviceRender->Start();
			if (FAILED(hResult))
			{
				return false;
			}

			//Return result
			std::cout << "Audio render device started." << std::endl;
			return true;
		}
		catch (...)
		{
			std::cout << "SetAudioDeviceRender failed." << std::endl;
			return false;
		}
	}

	BOOL SetAudioDeviceCapture()
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
			hResult = iDevice->Activate(__uuidof(IAudioClient3), CLSCTX_ALL, NULL, (LPVOID*)&iAudioDeviceCapture);
			if (FAILED(hResult))
			{
				return false;
			}

			//Get audio wave format information
			hResult = iAudioDeviceCapture->GetMixFormat((WAVEFORMATEX**)&iAudioWaveFormatExCapture);
			if (FAILED(hResult))
			{
				return false;
			}

			//Set audio wave format information
			iAudioWaveFormatExCapture->SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
			iAudioWaveFormatExCapture->Format.wBitsPerSample = vMediaSettings.AudioBitDepth;
			iAudioWaveFormatExCapture->Format.nSamplesPerSec = vMediaSettings.AudioSampleRate;
			iAudioWaveFormatExCapture->Format.nChannels = vMediaSettings.AudioChannels;
			iAudioWaveFormatExCapture->Format.nBlockAlign = iAudioWaveFormatExCapture->Format.nChannels * iAudioWaveFormatExCapture->Format.wBitsPerSample / 8;
			iAudioWaveFormatExCapture->Format.nAvgBytesPerSec = iAudioWaveFormatExCapture->Format.nSamplesPerSec * iAudioWaveFormatExCapture->Format.nBlockAlign;
			iAudioWaveFormatExCapture->Samples.wValidBitsPerSample = iAudioWaveFormatExCapture->Format.wBitsPerSample;

			//Initialize default audio device
			UINT initPeriodicity = 0;
			UINT initBufferDuration = 0;
			DWORD initFlags = AUDCLNT_STREAMFLAGS_LOOPBACK | AUDCLNT_STREAMFLAGS_NOPERSIST | AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM | AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY;
			hResult = iAudioDeviceCapture->Initialize(AUDCLNT_SHAREMODE_SHARED, initFlags, initBufferDuration, initPeriodicity, (WAVEFORMATEX*)iAudioWaveFormatExCapture.m_pData, 0);
			if (FAILED(hResult))
			{
				return false;
			}

			//Get audio device service
			hResult = iAudioDeviceCapture->GetService(IID_PPV_ARGS(&iAudioClientCapture));
			if (FAILED(hResult))
			{
				return false;
			}

			//Start audio device
			hResult = iAudioDeviceCapture->Start();
			if (FAILED(hResult))
			{
				return false;
			}

			//Return result
			std::cout << "Audio capture device started." << std::endl;
			return true;
		}
		catch (...)
		{
			std::cout << "SetAudioDeviceCapture failed." << std::endl;
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
			if (vMediaSettings.AudioFormat == MP3)
			{
				imfMediaTypeAudioOut->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_MP3);
			}

			//AAC
			if (vMediaSettings.AudioFormat == AAC)
			{
				imfMediaTypeAudioOut->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_AAC);
				imfMediaTypeAudioOut->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, iAudioWaveFormatExCapture->Format.wBitsPerSample);
			}

			//FLAC
			if (vMediaSettings.AudioFormat == FLAC)
			{
				imfMediaTypeAudioOut->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_FLAC);
				imfMediaTypeAudioOut->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, iAudioWaveFormatExCapture->Format.wBitsPerSample);
			}

			imfMediaTypeAudioOut->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
			imfMediaTypeAudioOut->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, iAudioWaveFormatExCapture->Format.nChannels);
			imfMediaTypeAudioOut->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, iAudioWaveFormatExCapture->Format.nSamplesPerSec);
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
			imfMediaTypeAudioIn->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, iAudioWaveFormatExCapture->Format.wBitsPerSample);
			imfMediaTypeAudioIn->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, iAudioWaveFormatExCapture->Format.nChannels);
			imfMediaTypeAudioIn->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, iAudioWaveFormatExCapture->Format.nSamplesPerSec);
			imfMediaTypeAudioIn->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, iAudioWaveFormatExCapture->Format.nAvgBytesPerSec);
			imfMediaTypeAudioIn->SetUINT32(MF_MT_AUDIO_BLOCK_ALIGNMENT, iAudioWaveFormatExCapture->Format.nBlockAlign);
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