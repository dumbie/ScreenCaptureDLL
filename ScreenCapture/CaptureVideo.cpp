#pragma once
#include "CaptureVariables.h"

namespace
{
	BOOL SetVideoEncoderDetails()
	{
		try
		{
			//Get service for stream
			CComPtr<ICodecAPI> iCodecApi;
			hResult = imfSinkWriter->GetServiceForStream(vOutVideoStreamIndex, GUID_NULL, IID_PPV_ARGS(&iCodecApi));
			if (FAILED(hResult))
			{
				std::cout << "GetServiceForStream video failed." << std::endl;
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
			std::cout << "SetVideoEncoderDetails failed." << std::endl;
			return false;
		}
	}

	BOOL SetVideoMediaType()
	{
		try
		{
			//Create video out media type
			CComPtr<IMFMediaType> imfMediaTypeVideoOut;
			hResult = MFCreateMediaType(&imfMediaTypeVideoOut);
			if (FAILED(hResult))
			{
				std::cout << "MFCreateMediaType video out failed." << std::endl;
				return false;
			}

			//Calculate video out bitrate
			UINT32 videoBitrate = 10000000 * vMediaSettings.VideoQuality;

			imfMediaTypeVideoOut->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
			imfMediaTypeVideoOut->SetGUID(MF_MT_SUBTYPE, vMediaSettings.VideoFormat);
			imfMediaTypeVideoOut->SetUINT32(MF_MT_AVG_BITRATE, videoBitrate);
			imfMediaTypeVideoOut->SetUINT32(MF_MT_ALL_SAMPLES_INDEPENDENT, 1);
			imfMediaTypeVideoOut->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive);
			MFSetAttributeSize(imfMediaTypeVideoOut, MF_MT_FRAME_SIZE, vCaptureDetails.Width, vCaptureDetails.Height);
			MFSetAttributeRatio(imfMediaTypeVideoOut, MF_MT_FRAME_RATE, vMediaSettings.VideoFrameRate, 1);
			MFSetAttributeRatio(imfMediaTypeVideoOut, MF_MT_PIXEL_ASPECT_RATIO, 1, 1);
			hResult = imfSinkWriter->AddStream(imfMediaTypeVideoOut, &vOutVideoStreamIndex);
			if (FAILED(hResult))
			{
				std::cout << "AddStream video failed." << std::endl;
				return false;
			}

			//Create video in media type
			CComPtr<IMFMediaType> imfMediaTypeVideoIn;
			hResult = MFCreateMediaType(&imfMediaTypeVideoIn);
			if (FAILED(hResult))
			{
				std::cout << "MFCreateMediaType video in failed." << std::endl;
				return false;
			}

			imfMediaTypeVideoIn->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
			imfMediaTypeVideoIn->SetUINT32(MF_MT_ALL_SAMPLES_INDEPENDENT, 1);
			MFSetAttributeSize(imfMediaTypeVideoIn, MF_MT_FRAME_SIZE, vCaptureDetails.Width, vCaptureDetails.Height);
			MFSetAttributeRatio(imfMediaTypeVideoIn, MF_MT_FRAME_RATE, vCaptureDetails.RefreshRate, 1);
			MFSetAttributeRatio(imfMediaTypeVideoIn, MF_MT_PIXEL_ASPECT_RATIO, 1, 1);
			//HDR and SDR settings
			if (vCaptureDetails.HDREnabled && !vCaptureSettings.HDRtoSDR)
			{
				std::cout << "Set media type format to HDR." << std::endl;
				imfMediaTypeVideoIn->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_A16B16G16R16F);
			}
			else
			{
				std::cout << "Set media type format to SDR." << std::endl;
				imfMediaTypeVideoIn->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB32);
			}
			hResult = imfSinkWriter->SetInputMediaType(vOutVideoStreamIndex, imfMediaTypeVideoIn, NULL);
			if (FAILED(hResult))
			{
				std::cout << "SetInputMediaType video failed." << std::endl;
				return false;
			}

			return true;
		}
		catch (...)
		{
			std::cout << "SetVideoMediaType failed." << std::endl;
			return false;
		}
	}
}