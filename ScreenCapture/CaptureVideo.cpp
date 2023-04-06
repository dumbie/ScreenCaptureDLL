#pragma once
#include "CaptureVariables.h"

namespace
{
	BOOL SetVideoEncoderDetails(CComPtr<IMFSinkWriterEx> imfSinkWriter)
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

	BOOL SetVideoTransformDetails(CComPtr<IMFSinkWriterEx> imfSinkWriter)
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
			imfMediaTypeVideoOut->SetGUID(MF_MT_SUBTYPE, vMediaSettings.VideoFormat);
			imfMediaTypeVideoOut->SetUINT32(MF_MT_AVG_BITRATE, vMediaSettings.VideoBitRate * 1000);
			imfMediaTypeVideoOut->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive);
			MFSetAttributeSize(imfMediaTypeVideoOut, MF_MT_FRAME_SIZE, vCaptureDetails.Width, vCaptureDetails.Height);
			MFSetAttributeRatio(imfMediaTypeVideoOut, MF_MT_FRAME_RATE, vMediaSettings.VideoFrameRate, 1);
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
			MFSetAttributeRatio(imfMediaTypeVideoIn, MF_MT_FRAME_RATE, vMediaSettings.VideoFrameRate, 1);
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
}