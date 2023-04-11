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
			ULONGLONG mediaTimeStartAudio = 0;
			ULONGLONG mediaTimeStartVideo = 0;
			while (vMediaWriteLoopAllowed)
			{
				//Update screen bytes and check cache
				UpdateScreenBytesCache(false);
				ULONGLONG mediaTimeDurationVideo = vReferenceTimeToSeconds / vMediaSettings.VideoFrameRate;
				if (vScreenBytesCache == NULL)
				{
					continue;
				}
				WriteMediaDataBytes(imfSinkWriter, vScreenBytesCache, vCaptureDetails.TotalByteSize, vOutVideoStreamIndex, mediaTimeStartVideo, mediaTimeDurationVideo);
				mediaTimeStartVideo += mediaTimeDurationVideo;

				////Get audio bytes
				//MediaFrameBytes mediaFrameAudio = CaptureAudioBytes();
				//ULONGLONG mediaTimeDurationAudio = mediaFrameAudio.TimeDuration;
				//if (mediaFrameAudio.Bytes == NULL)
				//{
				//	continue;
				//}
				//WriteMediaDataBytes(imfSinkWriter, mediaFrameAudio.Bytes, mediaFrameAudio.Size, vOutAudioStreamIndex, mediaTimeStartAudio, mediaTimeDurationAudio);
				//mediaTimeStartAudio += mediaTimeDurationAudio;
			}
		}
		catch (...) {}
		vMediaWriteLoopFinished = true;
	}

	BOOL InitializeMediaFoundation()
	{
		try
		{
			//Initialize media foundation
			MFStartup(MFSTARTUP_FULL); //MFSTARTUP_LITE

			//Create IMF attributes
			CComPtr<IMFAttributes> imfAttributes;
			hResult = MFCreateAttributes(&imfAttributes, 0);
			imfAttributes->SetUINT32(MF_READWRITE_ENABLE_HARDWARE_TRANSFORMS, true);
			imfAttributes->SetGUID(MF_TRANSCODE_CONTAINERTYPE, MFTranscodeContainerType_MPEG4);

			//Create IMF sink writer
			CComPtr<IMFSinkWriter> imfSinkWriterNormal;
			hResult = MFCreateSinkWriterFromURL(vMediaSettings.FileName, NULL, imfAttributes, &imfSinkWriterNormal);
			if (FAILED(hResult))
			{
				return false;
			}

			//Convert variables
			imfSinkWriterNormal->QueryInterface(&imfSinkWriter);

			//Set video media type
			SetVideoMediaType(imfSinkWriter);

			//Set video encoder details
			SetVideoEncoderDetails(imfSinkWriter);

			//Set video transform details
			SetVideoTransformDetails(imfSinkWriter);

			////Set audio device
			//if (!SetAudioDevice(imfSinkWriter)) { return false; }

			////Set audio media type
			//SetAudioMediaType(imfSinkWriter);

			return true;
		}
		catch (...)
		{
			return false;
		}
	}
}