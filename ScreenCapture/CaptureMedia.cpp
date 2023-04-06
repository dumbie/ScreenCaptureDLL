#pragma once
#include "CaptureVariables.h"
#include "CaptureScreen.cpp"
#include "CaptureVideo.cpp"
#include "CaptureAudio.cpp"

namespace
{
	BOOL WriteMediaDataBytes(CComPtr<IMFSinkWriterEx> imfSinkWriter, BYTE* mediaBytes, UINT mediaSize, UINT mediaIndex, LONGLONG mediaTimeStart, LONGLONG mediaTimeDuration)
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

			//Set media bytes
			BYTE* bytesMediaBuffer = new BYTE[mediaSize];
			imfMediaBuffer->Lock(&bytesMediaBuffer, NULL, NULL);
			memcpy(bytesMediaBuffer, mediaBytes, mediaSize);
			imfMediaBuffer->SetCurrentLength(mediaSize);
			imfMediaBuffer->Unlock();

			//Create media sample
			CComPtr<IMFSample> imfMediaSample;
			hResult = MFCreateSample(&imfMediaSample);
			imfMediaSample->AddBuffer(imfMediaBuffer);
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
			//delete[] bytesMediaBuffer;
			//imfMediaSample->RemoveAllBuffers();
			imfMediaSample.Release();
			imfMediaBuffer.Release();

			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	BOOL WriteMediaLoop(CComPtr<IMFSinkWriterEx> imfSinkWriter)
	{
		try
		{
			ULONGLONG mediaTimeStart = 0;
			while (vMediaWriteLoop)
			{
				//Get audio bytes
				MediaFrameBytes mediaFrameAudio = CaptureAudioBytes();
				if (mediaFrameAudio.TimeDuration == 0 || mediaFrameAudio.TimeDuration < vReferenceTimeMediaFrame) { continue; }
				ULONGLONG mediaTimeDuration = mediaFrameAudio.TimeDuration;

				//Get screen bytes
				BYTE* dataScreen = CaptureScreenBytes();
				//ULONGLONG mediaTimeDuration = vReferenceTimeToSeconds / vMediaSettings.VideoFrameRate;

				//Write media data bytes
				WriteMediaDataBytes(imfSinkWriter, dataScreen, vCaptureDetails.TotalByteSize, vOutVideoStreamIndex, mediaTimeStart, mediaTimeDuration);
				WriteMediaDataBytes(imfSinkWriter, mediaFrameAudio.Bytes, mediaFrameAudio.Size, vOutAudioStreamIndex, mediaTimeStart, mediaTimeDuration);

				//Release resources
				delete[] dataScreen;
				//delete[] mediaFrameAudio.Bytes;

				//Update media time
				mediaTimeStart += mediaTimeDuration;
			}

			return true;
		}
		catch (...) {}
		return false;
	}

	BOOL InitializeMediaFoundation()
	{
		try
		{
			//Update variables
			vMediaCapturing = true;

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

			//Convert IMF sink writer
			imfSinkWriterNormal->QueryInterface(&imfSinkWriter);

			//Set video media type
			SetVideoMediaType(imfSinkWriter);

			//Set video encoder details
			SetVideoEncoderDetails(imfSinkWriter);

			//Set video transform details
			SetVideoTransformDetails(imfSinkWriter);

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