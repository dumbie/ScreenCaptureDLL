#pragma once
#include "CaptureVariables.h"
#include "CaptureScreen.cpp"

namespace
{
	BOOL SetEncoderDetails(CComPtr<IMFSinkWriterEx> imfSinkWriter)
	{
		try
		{
			//Get service for stream
			CComPtr<ICodecAPI> iCodecApi;
			hResult = imfSinkWriter->GetServiceForStream(vOutVideoStreamIndex, GUID_NULL, IID_PPV_ARGS(&iCodecApi));
			if (FAILED(hResult))
			{
				return false;
			}

			//Set encoder common rate control
			VARIANT encCommonRateControl{};
			encCommonRateControl.vt = VT_UI4;
			encCommonRateControl.ulVal = eAVEncCommonRateControlMode_CBR;
			iCodecApi->SetValue(&CODECAPI_AVEncCommonRateControlMode, &encCommonRateControl);

			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	BOOL SetTransformDetails(CComPtr<IMFSinkWriterEx> imfSinkWriter)
	{
		try
		{
			GUID transformType;
			DWORD transformIndex = 0;
			CComPtr<IMFVideoProcessorControl> imfVideoProcessorControl;
			while (true)
			{
				CComPtr<IMFTransform> imfTransform;
				hResult = imfSinkWriter->GetTransformForStream(vOutVideoStreamIndex, transformIndex, &transformType, &imfTransform);
				if (transformType == MFT_CATEGORY_VIDEO_PROCESSOR)
				{
					imfTransform->QueryInterface(&imfVideoProcessorControl);
					break;
				}
				transformIndex++;
			}

			if (imfVideoProcessorControl)
			{
				imfVideoProcessorControl->SetMirror(MIRROR_VERTICAL);
			}

			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	BOOL SetVideoMediaType(CComPtr<IMFSinkWriterEx> imfSinkWriter)
	{
		try
		{
			//Create video out media type
			CComPtr<IMFMediaType> imfMediaTypeVideoOut;
			hResult = MFCreateMediaType(&imfMediaTypeVideoOut);
			if (FAILED(hResult))
			{
				return false;
			}

			imfMediaTypeVideoOut->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
			imfMediaTypeVideoOut->SetGUID(MF_MT_SUBTYPE, vVideoSettings.VideoFormat);
			imfMediaTypeVideoOut->SetUINT32(MF_MT_AVG_BITRATE, vVideoSettings.VideoBitRate * 1000);
			MFSetAttributeSize(imfMediaTypeVideoOut, MF_MT_FRAME_SIZE, vCaptureDetails.Width, vCaptureDetails.Height);
			MFSetAttributeRatio(imfMediaTypeVideoOut, MF_MT_FRAME_RATE, vVideoSettings.VideoFrameRate, 1);
			MFSetAttributeRatio(imfMediaTypeVideoOut, MF_MT_PIXEL_ASPECT_RATIO, 1, 1);
			hResult = imfSinkWriter->AddStream(imfMediaTypeVideoOut, &vOutVideoStreamIndex);
			if (FAILED(hResult))
			{
				return false;
			}

			//Create video in media type
			CComPtr<IMFMediaType> imfMediaTypeVideoIn;
			hResult = MFCreateMediaType(&imfMediaTypeVideoIn);
			if (FAILED(hResult))
			{
				return false;
			}

			imfMediaTypeVideoIn->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
			imfMediaTypeVideoIn->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_ARGB32); //SDR
			//imfMediaTypeVideoIn->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_A16B16G16R16F); //HDR
			MFSetAttributeSize(imfMediaTypeVideoIn, MF_MT_FRAME_SIZE, vCaptureDetails.Width, vCaptureDetails.Height);
			MFSetAttributeRatio(imfMediaTypeVideoIn, MF_MT_FRAME_RATE, vVideoSettings.VideoFrameRate, 1);
			MFSetAttributeRatio(imfMediaTypeVideoIn, MF_MT_PIXEL_ASPECT_RATIO, 1, 1);
			hResult = imfSinkWriter->SetInputMediaType(vOutVideoStreamIndex, imfMediaTypeVideoIn, NULL);
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

	BOOL WriteVideoAudioMedia(CComPtr<IMFSinkWriterEx> imfSinkWriter)
	{
		try
		{
			//Begin media write
			hResult = imfSinkWriter->BeginWriting();
			if (FAILED(hResult))
			{
				return false;
			}

			//Create media buffer
			CComPtr<IMFMediaBuffer> imfBufferVideo;
			hResult = MFCreateMemoryBuffer(vCaptureDetails.TotalByteSize, &imfBufferVideo);
			if (FAILED(hResult))
			{
				return false;
			}

			//Create media sample
			CComPtr<IMFSample> imfSampleVideo;
			hResult = MFCreateSample(&imfSampleVideo);
			imfSampleVideo->RemoveAllBuffers();
			imfSampleVideo->AddBuffer(imfBufferVideo);
			if (FAILED(hResult))
			{
				return false;
			}

			//Set media duration
			LONGLONG durationVideo = 10 * 1000 * 1000 / vVideoSettings.VideoFrameRate;
			hResult = imfSampleVideo->SetSampleDuration(durationVideo);
			if (FAILED(hResult))
			{
				return false;
			}

			//Write media to sample
			vVideoWriteLoop = true;
			while (vVideoWriteLoop)
			{
				BYTE* bytesBuffer;
				BYTE* bytesScreen = CaptureScreenBytes();
				imfBufferVideo->Lock(&bytesBuffer, NULL, NULL);
				memcpy(bytesBuffer, bytesScreen, vCaptureDetails.TotalByteSize);
				imfBufferVideo->SetCurrentLength(vCaptureDetails.TotalByteSize);
				imfBufferVideo->Unlock();

				imfSinkWriter->WriteSample(vOutVideoStreamIndex, imfSampleVideo);
				delete[] bytesScreen;
			}

			//Finalize media write
			hResult = imfSinkWriter->Finalize();
			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	BOOL InitializeMediaFoundation()
	{
		try
		{
			//Update variables
			vVideoCapturing = true;

			//Initialize media foundation
			MFStartup(MFSTARTUP_FULL); //MFSTARTUP_LITE

			//Create IMF attributes
			CComPtr<IMFAttributes> imfAttributes;
			hResult = MFCreateAttributes(&imfAttributes, 0);
			imfAttributes->SetUINT32(MF_READWRITE_ENABLE_HARDWARE_TRANSFORMS, true);
			imfAttributes->SetGUID(MF_TRANSCODE_CONTAINERTYPE, MFTranscodeContainerType_MPEG4);

			//Create IMF sink writer
			CComPtr<IMFSinkWriter> imfSinkWriterNormal;
			hResult = MFCreateSinkWriterFromURL(vVideoSettings.FileName, NULL, imfAttributes, &imfSinkWriterNormal);

			//Convert IMF sink writer
			CComPtr<IMFSinkWriterEx> imfSinkWriter;
			imfSinkWriterNormal->QueryInterface(&imfSinkWriter);

			//Set video media type
			SetVideoMediaType(imfSinkWriter);

			//Set encoder details
			SetEncoderDetails(imfSinkWriter);

			//Set transform details
			SetTransformDetails(imfSinkWriter);

			//Set audio media type

			//Write video and audio stream
			WriteVideoAudioMedia(imfSinkWriter);

			return true;
		}
		catch (...)
		{
			return false;
		}
	}
}