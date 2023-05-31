#pragma once
#include "CaptureVariables.h"

namespace
{
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

			//Set video format
			GUID videoFormat{};
			if (vMediaSettings.VideoFormat == H264)
			{
				videoFormat = MFVideoFormat_H264;
			}
			else if (vMediaSettings.VideoFormat == HEVC)
			{
				videoFormat = MFVideoFormat_HEVC;
			}

			//Check if HDR is enabled
			BOOL hdrEnabled = vCaptureDetails.HDREnabled && !vCaptureSettings.HDRtoSDR;
			if (hdrEnabled)
			{
				videoFormat = MFVideoFormat_HEVC;
			}

			//Set encoder settings
			if (videoFormat == MFVideoFormat_HEVC)
			{
				imfMediaTypeVideoOut->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_HEVC);
				imfMediaTypeVideoOut->SetUINT32(MF_MT_VIDEO_LEVEL, eAVEncH265VLevel5);
				//HDR and SDR settings
				if (hdrEnabled)
				{
					imfMediaTypeVideoOut->SetUINT32(MF_MT_VIDEO_PROFILE, eAVEncH265VProfile_Main_420_10);
					//imfMediaTypeVideoOut->SetUINT32(MF_MT_VIDEO_PRIMARIES, MFVideoPrimaries_BT2020);
					//imfMediaTypeVideoOut->SetUINT32(MF_MT_TRANSFER_FUNCTION, MFVideoTransFunc_2084); //PQ
					//imfMediaTypeVideoOut->SetUINT32(MF_MT_YUV_MATRIX, MFVideoTransferMatrix_BT2020_10);
				}
				else
				{
					imfMediaTypeVideoOut->SetUINT32(MF_MT_VIDEO_PROFILE, eAVEncH265VProfile_Main_420_8);
				}
			}
			else
			{
				imfMediaTypeVideoOut->SetGUID(MF_MT_SUBTYPE, videoFormat);
				imfMediaTypeVideoOut->SetUINT32(MF_MT_VIDEO_LEVEL, eAVEncH264VLevel5);
				imfMediaTypeVideoOut->SetUINT32(MF_MT_VIDEO_PROFILE, eAVEncH264VProfile_Main);
			}

			imfMediaTypeVideoOut->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
			imfMediaTypeVideoOut->SetUINT32(MF_MT_ALL_SAMPLES_INDEPENDENT, 1);
			imfMediaTypeVideoOut->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive);
			MFSetAttributeSize(imfMediaTypeVideoOut, MF_MT_FRAME_SIZE, vCaptureDetails.OutputWidth, vCaptureDetails.OutputHeight);
			MFSetAttributeRatio(imfMediaTypeVideoOut, MF_MT_FRAME_RATE, vMediaSettings.VideoFrameRate, 1);
			MFSetAttributeRatio(imfMediaTypeVideoOut, MF_MT_PIXEL_ASPECT_RATIO, 1, 1);
			hResult = imfSinkWriter->AddStream(imfMediaTypeVideoOut, &vOutVideoStreamIndex);
			if (FAILED(hResult))
			{
				std::cout << "AddStream video failed." << std::endl;
				return false;
			}

			//Create video encoding parameters
			CComPtr<IMFAttributes> imfAttributesEncoding;
			MFCreateAttributes(&imfAttributesEncoding, 0);
			imfAttributesEncoding->SetUINT32(CODECAPI_AVEncCommonRateControlMode, eAVEncCommonRateControlMode_Quality);
			imfAttributesEncoding->SetUINT32(CODECAPI_AVEncCommonQuality, vMediaSettings.VideoQuality);
			imfAttributesEncoding->SetUINT32(CODECAPI_AVEncCommonQualityVsSpeed, 80);
			imfAttributesEncoding->SetUINT32(CODECAPI_AVEncCommonRealTime, 1);
			imfAttributesEncoding->SetUINT32(CODECAPI_AVLowLatencyMode, 1);

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
			MFSetAttributeSize(imfMediaTypeVideoIn, MF_MT_FRAME_SIZE, vCaptureDetails.OutputWidth, vCaptureDetails.OutputHeight);
			MFSetAttributeRatio(imfMediaTypeVideoIn, MF_MT_FRAME_RATE, vCaptureDetails.RefreshRate, 1);
			MFSetAttributeRatio(imfMediaTypeVideoIn, MF_MT_PIXEL_ASPECT_RATIO, 1, 1);
			//HDR and SDR settings
			if (hdrEnabled)
			{
				std::cout << "Set media type format to HDR." << std::endl;
				imfMediaTypeVideoIn->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_A16B16G16R16F);
			}
			else
			{
				std::cout << "Set media type format to SDR." << std::endl;
				imfMediaTypeVideoIn->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_ARGB32);
			}
			hResult = imfSinkWriter->SetInputMediaType(vOutVideoStreamIndex, imfMediaTypeVideoIn, imfAttributesEncoding);
			if (FAILED(hResult))
			{
				std::cout << "SetInputMediaType video failed: " << hResult << std::endl;
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