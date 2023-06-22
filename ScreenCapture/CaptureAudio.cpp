#pragma once
#include "CaptureVariables.h"

namespace
{
	std::vector<BYTE> GetAudioBytes(UINT captureInstanceId)
	{
		try
		{
			//Get audio buffer
			DWORD mediaFlags;
			UINT32 mediaFramesRead;
			BYTE* mediaBuffer;
			UINT64 devicePosition;
			UINT64 qpcPosition;
			hResult = vCaptureInstances[captureInstanceId].iAudioClientCapture->GetBuffer(&mediaBuffer, &mediaFramesRead, &mediaFlags, &devicePosition, &qpcPosition);
			if (FAILED(hResult))
			{
				return {};
			}

			//Release audio buffer
			hResult = vCaptureInstances[captureInstanceId].iAudioClientCapture->ReleaseBuffer(mediaFramesRead);
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
			UINT32 mediaFramesTarget = vCaptureInstances[captureInstanceId].iAudioWaveFormatExCapture->Format.nSamplesPerSec / 100;

			//Check media frames read
			if (mediaFramesRead == mediaFramesTarget)
			{
				//std::cout << "Writing read audio bytes: " << mediaFramesRead << "/" << devicePosition << "/" << qpcPosition << "/" << mediaFlags << std::endl;

				//Calculate media size
				UINT audioBytesSize = mediaFramesRead * vCaptureInstances[captureInstanceId].iAudioWaveFormatExCapture->Format.nBlockAlign;

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
				UINT audioBytesSize = mediaFramesTarget * vCaptureInstances[captureInstanceId].iAudioWaveFormatExCapture->Format.nBlockAlign;

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

	BOOL SetAudioDeviceRender(UINT captureInstanceId)
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
			hResult = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &vCaptureInstances[captureInstanceId].iDevice);
			if (FAILED(hResult))
			{
				return false;
			}

			//Activate default audio device
			hResult = vCaptureInstances[captureInstanceId].iDevice->Activate(__uuidof(IAudioClient3), CLSCTX_ALL, NULL, (LPVOID*)&vCaptureInstances[captureInstanceId].iAudioDeviceRender);
			if (FAILED(hResult))
			{
				return false;
			}

			//Get audio wave format information
			hResult = vCaptureInstances[captureInstanceId].iAudioDeviceRender->GetMixFormat((WAVEFORMATEX**)&vCaptureInstances[captureInstanceId].iAudioWaveFormatExRender);
			if (FAILED(hResult))
			{
				return false;
			}

			//Initialize default audio device
			UINT initPeriodicity = 0;
			UINT initBufferDuration = 0;
			DWORD initFlags = 0;
			hResult = vCaptureInstances[captureInstanceId].iAudioDeviceRender->Initialize(AUDCLNT_SHAREMODE_SHARED, initFlags, initBufferDuration, initPeriodicity, (WAVEFORMATEX*)vCaptureInstances[captureInstanceId].iAudioWaveFormatExRender.m_pData, 0);
			if (FAILED(hResult))
			{
				return false;
			}

			//Get audio device service
			hResult = vCaptureInstances[captureInstanceId].iAudioDeviceRender->GetService(IID_PPV_ARGS(&vCaptureInstances[captureInstanceId].iAudioClientRender));
			if (FAILED(hResult))
			{
				return false;
			}

			//Start audio device
			hResult = vCaptureInstances[captureInstanceId].iAudioDeviceRender->Start();
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

	BOOL SetAudioDeviceCapture(UINT captureInstanceId)
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
			hResult = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &vCaptureInstances[captureInstanceId].iDevice);
			if (FAILED(hResult))
			{
				return false;
			}

			//Activate default audio device
			hResult = vCaptureInstances[captureInstanceId].iDevice->Activate(__uuidof(IAudioClient3), CLSCTX_ALL, NULL, (LPVOID*)&vCaptureInstances[captureInstanceId].iAudioDeviceCapture);
			if (FAILED(hResult))
			{
				return false;
			}

			//Get audio wave format information
			hResult = vCaptureInstances[captureInstanceId].iAudioDeviceCapture->GetMixFormat((WAVEFORMATEX**)&vCaptureInstances[captureInstanceId].iAudioWaveFormatExCapture);
			if (FAILED(hResult))
			{
				return false;
			}

			//Set audio wave format information
			vCaptureInstances[captureInstanceId].iAudioWaveFormatExCapture->SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
			vCaptureInstances[captureInstanceId].iAudioWaveFormatExCapture->Format.wBitsPerSample = vCaptureInstances[captureInstanceId].vMediaSettings.AudioBitDepth;
			vCaptureInstances[captureInstanceId].iAudioWaveFormatExCapture->Format.nSamplesPerSec = vCaptureInstances[captureInstanceId].vMediaSettings.AudioSampleRate;
			vCaptureInstances[captureInstanceId].iAudioWaveFormatExCapture->Format.nChannels = vCaptureInstances[captureInstanceId].vMediaSettings.AudioChannels;
			vCaptureInstances[captureInstanceId].iAudioWaveFormatExCapture->Format.nBlockAlign = vCaptureInstances[captureInstanceId].iAudioWaveFormatExCapture->Format.nChannels * vCaptureInstances[captureInstanceId].iAudioWaveFormatExCapture->Format.wBitsPerSample / 8;
			vCaptureInstances[captureInstanceId].iAudioWaveFormatExCapture->Format.nAvgBytesPerSec = vCaptureInstances[captureInstanceId].iAudioWaveFormatExCapture->Format.nSamplesPerSec * vCaptureInstances[captureInstanceId].iAudioWaveFormatExCapture->Format.nBlockAlign;
			vCaptureInstances[captureInstanceId].iAudioWaveFormatExCapture->Samples.wValidBitsPerSample = vCaptureInstances[captureInstanceId].iAudioWaveFormatExCapture->Format.wBitsPerSample;

			//Initialize default audio device
			UINT initPeriodicity = 0;
			UINT initBufferDuration = 0;
			DWORD initFlags = AUDCLNT_STREAMFLAGS_LOOPBACK | AUDCLNT_STREAMFLAGS_NOPERSIST | AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM | AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY;
			hResult = vCaptureInstances[captureInstanceId].iAudioDeviceCapture->Initialize(AUDCLNT_SHAREMODE_SHARED, initFlags, initBufferDuration, initPeriodicity, (WAVEFORMATEX*)vCaptureInstances[captureInstanceId].iAudioWaveFormatExCapture.m_pData, 0);
			if (FAILED(hResult))
			{
				return false;
			}

			//Get audio device service
			hResult = vCaptureInstances[captureInstanceId].iAudioDeviceCapture->GetService(IID_PPV_ARGS(&vCaptureInstances[captureInstanceId].iAudioClientCapture));
			if (FAILED(hResult))
			{
				return false;
			}

			//Start audio device
			hResult = vCaptureInstances[captureInstanceId].iAudioDeviceCapture->Start();
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

	BOOL SetAudioMediaType(UINT captureInstanceId)
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
			if (vCaptureInstances[captureInstanceId].vMediaSettings.AudioFormat == MP3)
			{
				imfMediaTypeAudioOut->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_MP3);
			}

			//AAC
			if (vCaptureInstances[captureInstanceId].vMediaSettings.AudioFormat == AAC)
			{
				imfMediaTypeAudioOut->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_AAC);
				imfMediaTypeAudioOut->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, vCaptureInstances[captureInstanceId].iAudioWaveFormatExCapture->Format.wBitsPerSample);
			}

			//FLAC
			if (vCaptureInstances[captureInstanceId].vMediaSettings.AudioFormat == FLAC)
			{
				imfMediaTypeAudioOut->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_FLAC);
				imfMediaTypeAudioOut->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, vCaptureInstances[captureInstanceId].iAudioWaveFormatExCapture->Format.wBitsPerSample);
			}

			imfMediaTypeAudioOut->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
			imfMediaTypeAudioOut->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, vCaptureInstances[captureInstanceId].iAudioWaveFormatExCapture->Format.nChannels);
			imfMediaTypeAudioOut->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, vCaptureInstances[captureInstanceId].iAudioWaveFormatExCapture->Format.nSamplesPerSec);
			imfMediaTypeAudioOut->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, vCaptureInstances[captureInstanceId].vMediaSettings.AudioBitRate * 1000 / 8);
			hResult = vCaptureInstances[captureInstanceId].imfSinkWriter->AddStream(imfMediaTypeAudioOut, &vCaptureInstances[captureInstanceId].vOutAudioStreamIndex);
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
			imfMediaTypeAudioIn->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, vCaptureInstances[captureInstanceId].iAudioWaveFormatExCapture->Format.wBitsPerSample);
			imfMediaTypeAudioIn->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, vCaptureInstances[captureInstanceId].iAudioWaveFormatExCapture->Format.nChannels);
			imfMediaTypeAudioIn->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, vCaptureInstances[captureInstanceId].iAudioWaveFormatExCapture->Format.nSamplesPerSec);
			imfMediaTypeAudioIn->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, vCaptureInstances[captureInstanceId].iAudioWaveFormatExCapture->Format.nAvgBytesPerSec);
			imfMediaTypeAudioIn->SetUINT32(MF_MT_AUDIO_BLOCK_ALIGNMENT, vCaptureInstances[captureInstanceId].iAudioWaveFormatExCapture->Format.nBlockAlign);
			hResult = vCaptureInstances[captureInstanceId].imfSinkWriter->SetInputMediaType(vCaptureInstances[captureInstanceId].vOutAudioStreamIndex, imfMediaTypeAudioIn, NULL);
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