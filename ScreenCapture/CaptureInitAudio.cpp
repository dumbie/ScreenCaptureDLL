#pragma once
#include "CaptureVariables.h"

namespace
{
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
			hResult = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &vMediaFoundationInstance.iDevice);
			if (FAILED(hResult))
			{
				return false;
			}

			//Activate default audio device
			hResult = vMediaFoundationInstance.iDevice->Activate(__uuidof(IAudioClient3), CLSCTX_ALL, NULL, (LPVOID*)&vMediaFoundationInstance.iAudioDeviceRender);
			if (FAILED(hResult))
			{
				return false;
			}

			//Get audio wave format information
			hResult = vMediaFoundationInstance.iAudioDeviceRender->GetMixFormat((WAVEFORMATEX**)&vMediaFoundationInstance.iAudioWaveFormatExRender);
			if (FAILED(hResult))
			{
				return false;
			}

			//Initialize default audio device
			UINT initPeriodicity = 0;
			UINT initBufferDuration = 0;
			DWORD initFlags = 0;
			hResult = vMediaFoundationInstance.iAudioDeviceRender->Initialize(AUDCLNT_SHAREMODE_SHARED, initFlags, initBufferDuration, initPeriodicity, (WAVEFORMATEX*)vMediaFoundationInstance.iAudioWaveFormatExRender.m_pData, 0);
			if (FAILED(hResult))
			{
				return false;
			}

			//Get audio device service
			hResult = vMediaFoundationInstance.iAudioDeviceRender->GetService(IID_PPV_ARGS(&vMediaFoundationInstance.iAudioClientRender));
			if (FAILED(hResult))
			{
				return false;
			}

			//Start audio device
			hResult = vMediaFoundationInstance.iAudioDeviceRender->Start();
			if (FAILED(hResult))
			{
				return false;
			}

			//Return result
			AVDebugWriteLine("Audio render device started.");
			return true;
		}
		catch (...)
		{
			AVDebugWriteLine("SetAudioDeviceRender failed.");
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
			hResult = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &vMediaFoundationInstance.iDevice);
			if (FAILED(hResult))
			{
				return false;
			}

			//Activate default audio device
			hResult = vMediaFoundationInstance.iDevice->Activate(__uuidof(IAudioClient3), CLSCTX_ALL, NULL, (LPVOID*)&vMediaFoundationInstance.iAudioDeviceCapture);
			if (FAILED(hResult))
			{
				return false;
			}

			//Set audio wave format information
			vMediaFoundationInstance.iAudioWaveFormatExCapture.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
			vMediaFoundationInstance.iAudioWaveFormatExCapture.dwChannelMask = SPEAKER_ALL;
			vMediaFoundationInstance.iAudioWaveFormatExCapture.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
			vMediaFoundationInstance.iAudioWaveFormatExCapture.Format.wBitsPerSample = vMediaSettings.AudioBitDepth;
			vMediaFoundationInstance.iAudioWaveFormatExCapture.Format.nSamplesPerSec = vMediaSettings.AudioSampleRate;
			vMediaFoundationInstance.iAudioWaveFormatExCapture.Format.nChannels = vMediaSettings.AudioChannels;
			vMediaFoundationInstance.iAudioWaveFormatExCapture.Format.nBlockAlign = vMediaFoundationInstance.iAudioWaveFormatExCapture.Format.nChannels * vMediaFoundationInstance.iAudioWaveFormatExCapture.Format.wBitsPerSample / 8;
			vMediaFoundationInstance.iAudioWaveFormatExCapture.Format.nAvgBytesPerSec = vMediaFoundationInstance.iAudioWaveFormatExCapture.Format.nSamplesPerSec * vMediaFoundationInstance.iAudioWaveFormatExCapture.Format.nBlockAlign;
			vMediaFoundationInstance.iAudioWaveFormatExCapture.Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);
			vMediaFoundationInstance.iAudioWaveFormatExCapture.Samples.wValidBitsPerSample = vMediaFoundationInstance.iAudioWaveFormatExCapture.Format.wBitsPerSample;
			vMediaFoundationInstance.iAudioWaveFormatExCapture.Samples.wSamplesPerBlock = vMediaFoundationInstance.iAudioWaveFormatExCapture.Format.wBitsPerSample;
			vMediaFoundationInstance.iAudioWaveFormatExCapture.Samples.wReserved = vMediaFoundationInstance.iAudioWaveFormatExCapture.Format.wBitsPerSample;

			//Initialize default audio device
			UINT initPeriodicity = 0;
			UINT initBufferDuration = vReferenceTimeToSeconds;
			DWORD initFlags = AUDCLNT_STREAMFLAGS_LOOPBACK | AUDCLNT_STREAMFLAGS_NOPERSIST | AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM | AUDCLNT_STREAMFLAGS_RATEADJUST | AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY;
			hResult = vMediaFoundationInstance.iAudioDeviceCapture->Initialize(AUDCLNT_SHAREMODE_SHARED, initFlags, initBufferDuration, initPeriodicity, &vMediaFoundationInstance.iAudioWaveFormatExCapture.Format, 0);
			if (FAILED(hResult))
			{
				return false;
			}

			//Get audio device service
			hResult = vMediaFoundationInstance.iAudioDeviceCapture->GetService(IID_PPV_ARGS(&vMediaFoundationInstance.iAudioClientCapture));
			if (FAILED(hResult))
			{
				return false;
			}

			//Start audio device
			hResult = vMediaFoundationInstance.iAudioDeviceCapture->Start();
			if (FAILED(hResult))
			{
				return false;
			}

			//Return result
			AVDebugWriteLine("Audio capture device started.");
			return true;
		}
		catch (...)
		{
			AVDebugWriteLine("SetAudioDeviceCapture failed.");
			return false;
		}
	}

	BOOL SetAudioMediaTypeOut()
	{
		try
		{
			//Create audio out media type
			CComPtr<IMFMediaType> imfMediaTypeAudioOut;
			hResult = MFCreateMediaType(&imfMediaTypeAudioOut);
			if (FAILED(hResult))
			{
				AVDebugWriteLine("MFCreateMediaType audio out failed.");
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
				imfMediaTypeAudioOut->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, vMediaFoundationInstance.iAudioWaveFormatExCapture.Format.wBitsPerSample);
			}

			//FLAC
			if (vMediaSettings.AudioFormat == FLAC)
			{
				imfMediaTypeAudioOut->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_FLAC);
				imfMediaTypeAudioOut->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, vMediaFoundationInstance.iAudioWaveFormatExCapture.Format.wBitsPerSample);
			}

			imfMediaTypeAudioOut->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
			imfMediaTypeAudioOut->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, vMediaFoundationInstance.iAudioWaveFormatExCapture.Format.nChannels);
			imfMediaTypeAudioOut->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, vMediaFoundationInstance.iAudioWaveFormatExCapture.Format.nSamplesPerSec);
			imfMediaTypeAudioOut->SetUINT32(MF_MT_AUDIO_BLOCK_ALIGNMENT, vMediaFoundationInstance.iAudioWaveFormatExCapture.Format.nBlockAlign);
			imfMediaTypeAudioOut->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, vMediaSettings.AudioBitRate * 1000 / 8);
			hResult = vMediaFoundationInstance.imfSinkWriter->AddStream(imfMediaTypeAudioOut, &vMediaFoundationInstance.vOutAudioStreamIndex);
			if (FAILED(hResult))
			{
				AVDebugWriteLine("AddStream audio failed.");
				return false;
			}

			return true;
		}
		catch (...)
		{
			AVDebugWriteLine("SetAudioMediaTypeOut failed.");
			return false;
		}
	}

	BOOL SetAudioMediaTypeIn()
	{
		try
		{
			//Create audio in media type
			CComPtr<IMFMediaType> imfMediaTypeAudioIn;
			hResult = MFCreateMediaType(&imfMediaTypeAudioIn);
			if (FAILED(hResult))
			{
				AVDebugWriteLine("MFCreateMediaType audio in failed.");
				return false;
			}

			hResult = MFInitMediaTypeFromWaveFormatEx(imfMediaTypeAudioIn, &vMediaFoundationInstance.iAudioWaveFormatExCapture.Format, sizeof(vMediaFoundationInstance.iAudioWaveFormatExCapture));
			if (FAILED(hResult))
			{
				AVDebugWriteLine("MFInitMediaTypeFromWaveFormatEx audio failed.");
				return false;
			}

			hResult = vMediaFoundationInstance.imfSinkWriter->SetInputMediaType(vMediaFoundationInstance.vOutAudioStreamIndex, imfMediaTypeAudioIn, NULL);
			if (FAILED(hResult))
			{
				AVDebugWriteLine("SetInputMediaType audio failed.");
				return false;
			}

			return true;
		}
		catch (...)
		{
			AVDebugWriteLine("SetAudioMediaTypeIn failed.");
			return false;
		}
	}
}