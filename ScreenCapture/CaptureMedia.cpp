#pragma once
#include "CaptureVariables.h"
#include "CaptureScreen.cpp"
#include "CaptureVideo.cpp"
#include "CaptureAudio.cpp"

namespace
{
	BOOL WriteMediaDataBytes(CComPtr<IMFSinkWriterEx> imfSinkWriter, BYTE* mediaBytes, UINT mediaSize, UINT mediaIndex, ULONGLONG mediaTimeStart, ULONGLONG mediaTimeDuration)
	{
		try
		{
			//Create media buffer
			CComPtr<IMFMediaBuffer> imfMediaBuffer;
			hResult = MFCreateMemoryBuffer(mediaSize, &imfMediaBuffer);
			if (FAILED(hResult))
			{
				return false;
			}

			//Lock media bytes
			BYTE* mediaBufferBytes;
			hResult = imfMediaBuffer->Lock(&mediaBufferBytes, NULL, NULL);
			if (FAILED(hResult))
			{
				return false;
			}

			//Set media bytes
			memcpy(mediaBufferBytes, mediaBytes, mediaSize);

			//Unlock media bytes
			hResult = imfMediaBuffer->Unlock();
			if (FAILED(hResult))
			{
				return false;
			}

			//Set media length
			hResult = imfMediaBuffer->SetCurrentLength(mediaSize);
			if (FAILED(hResult))
			{
				return false;
			}

			//Create media sample
			CComPtr<IMFSample> imfMediaSample;
			hResult = MFCreateSample(&imfMediaSample);
			if (FAILED(hResult))
			{
				return false;
			}

			//Add media buffer to sample
			hResult = imfMediaSample->AddBuffer(imfMediaBuffer);
			if (FAILED(hResult))
			{
				return false;
			}

			//Set media start time
			hResult = imfMediaSample->SetSampleTime(mediaTimeStart);
			if (FAILED(hResult))
			{
				return false;
			}

			//Set media duration time
			hResult = imfMediaSample->SetSampleDuration(mediaTimeDuration);
			if (FAILED(hResult))
			{
				return false;
			}

			//Write media to sample
			hResult = imfSinkWriter->WriteSample(mediaIndex, imfMediaSample);
			if (FAILED(hResult))
			{
				return false;
			}

			//Release resources
			imfMediaSample.Release();
			imfMediaBuffer.Release();

			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	VOID WriteMediaLoop(CComPtr<IMFSinkWriterEx> imfSinkWriter)
	{
		try
		{
			vMediaTimeLast = 0;
			vMediaTimeStart = 0;
			vMediaTimeDuration = 0;
			vAudioIsMuted = true;
			while (vMediaWriteLoopAllowed)
			{
				//Fix add code to delay loop when writing to fast

				//Update screen bytes and check
				UpdateScreenBytesCache(false, true);
				if (vScreenBytesCache.empty())
				{
					continue;
				}

				//Update audio bytes and check
				UpdateAudioBytesCache();
				if (vAudioBytesCache.empty())
				{
					continue;
				}

				//Write media data bytes
				WriteMediaDataBytes(imfSinkWriter, vScreenBytesCache.data(), vScreenBytesCache.size(), vOutVideoStreamIndex, vMediaTimeStart, vMediaTimeDuration);
				WriteMediaDataBytes(imfSinkWriter, vAudioBytesCache.data(), vAudioBytesCache.size(), vOutAudioStreamIndex, vMediaTimeStart, vMediaTimeDuration);
				//std::cout << "Written media at: " << vMediaTimeStart << " duration: " << vMediaTimeDuration << std::endl;

				//Update media time start
				vMediaTimeStart += vMediaTimeDuration;
			}
		}
		catch (...) {}
		vMediaWriteLoopFinished = true;
	}

	BOOL InitializeMediaFoundation(WCHAR* filePath)
	{
		try
		{
			//Initialize media foundation
			MFStartup(MFSTARTUP_LITE);

			//Create IMF attributes
			CComPtr<IMFAttributes> imfAttributes;
			hResult = MFCreateAttributes(&imfAttributes, 0);
			//imfAttributes->SetUINT32(MF_LOW_LATENCY, 1);
			imfAttributes->SetUINT32(MF_SINK_WRITER_DISABLE_THROTTLING, 1);
			imfAttributes->SetUINT32(MF_READWRITE_ENABLE_HARDWARE_TRANSFORMS, 1);
			imfAttributes->SetGUID(MF_TRANSCODE_CONTAINERTYPE, MFTranscodeContainerType_MPEG4);

			//Create IMF sink writer
			CComPtr<IMFSinkWriter> imfSinkWriterNormal;
			hResult = MFCreateSinkWriterFromURL(filePath, NULL, imfAttributes, &imfSinkWriterNormal);
			if (FAILED(hResult))
			{
				return false;
			}

			//Convert variables
			imfSinkWriterNormal->QueryInterface(&imfSinkWriter);

			//Set video encoder details
			SetVideoEncoderDetails(imfSinkWriter);

			//Set video media type
			SetVideoMediaType(imfSinkWriter);

			//Set audio device
			if (!SetAudioDevice(imfSinkWriter)) { return false; }

			//Set audio media type
			SetAudioMediaType(imfSinkWriter);

			return true;
		}
		catch (...)
		{
			return false;
		}
	}
}