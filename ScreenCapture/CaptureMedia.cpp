#pragma once
#include "CaptureVariables.h"
#include "CaptureScreen.cpp"
#include "CaptureVideo.cpp"
#include "CaptureAudio.cpp"

namespace
{
	BOOL WriteMediaDataBytes(CComPtr<IMFSinkWriterEx> imfSinkWriter, DataBytes dataBytes, UINT streamIndex)
	{
		try
		{
			//Create media buffer
			CComPtr<IMFMediaBuffer> imfBufferMedia;
			hResult = MFCreateMemoryBuffer(dataBytes.Size, &imfBufferMedia);
			if (FAILED(hResult))
			{
				return false;
			}

			//Create media sample
			CComPtr<IMFSample> imfSampleMedia;
			hResult = MFCreateSample(&imfSampleMedia);
			//imfSampleMedia->RemoveAllBuffers();
			imfSampleMedia->AddBuffer(imfBufferMedia);
			if (FAILED(hResult))
			{
				return false;
			}

			//Set media duration
			LONGLONG durationMedia = vReferenceTimeToSeconds / vMediaSettings.VideoFrameRate;
			hResult = imfSampleMedia->SetSampleDuration(durationMedia);
			if (FAILED(hResult))
			{
				return false;
			}

			//Set media bytes
			BYTE* bytesMediaBuffer;
			imfBufferMedia->Lock(&bytesMediaBuffer, NULL, NULL);
			//memset(bytesMediaBuffer, 0, dataBytes.Size);
			memcpy(bytesMediaBuffer, dataBytes.Bytes, dataBytes.Size);
			imfBufferMedia->SetCurrentLength(dataBytes.Size);
			imfBufferMedia->Unlock();

			//Write media to sample
			imfSinkWriter->WriteSample(streamIndex, imfSampleMedia);

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
			while (vMediaWriteLoop)
			{
				//Get, write and clear screen bytes
				DataBytes dataBytesScreen = CaptureScreenBytes();
				WriteMediaDataBytes(imfSinkWriter, dataBytesScreen, vOutVideoStreamIndex);
				delete[] dataBytesScreen.Bytes;

				////Get, write and clear audio bytes
				//DataBytes dataBytesAudio = CaptureAudioBytes();
				//WriteMediaDataBytes(imfSinkWriter, dataBytesAudio, vOutAudioStreamIndex);
				//delete[] dataBytesAudio.Bytes;
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