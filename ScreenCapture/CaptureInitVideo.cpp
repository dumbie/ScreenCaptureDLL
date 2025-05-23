#pragma once
#include "CaptureVariables.h"
#include "CaptureMonitorInfo.cpp"
#include "CaptureCodecCheck.cpp"

namespace
{
	CaptureResult SetVideoMediaType()
	{
		try
		{
			//Create video out media type
			CComPtr<IMFMediaType> imfMediaTypeVideoOut;
			hResult = MFCreateMediaType(&imfMediaTypeVideoOut);
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"MFCreateMediaType video out failed") };
			}

			//Set video format
			GUID videoFormat{};
			if (vMediaSettings.VideoFormat == H264)
			{
				videoFormat = MFVideoFormat_H264;
				AVDebugWriteLine("Set video output format to H264.");
			}
			else if (vMediaSettings.VideoFormat == HEVC)
			{
				videoFormat = MFVideoFormat_HEVC;
				AVDebugWriteLine("Set video output format to HEVC.");
			}
			else if (vMediaSettings.VideoFormat == AV1)
			{
				videoFormat = MFVideoFormat_AV1;
				AVDebugWriteLine("Set video output format to AV1.");
			}

			//Check if video format is supported
			if (!CodecCheckVideoEncoder(videoFormat))
			{
				return { .Status = CaptureStatus::Failed, .Message = SysAllocString(L"Video format is not supported") };
			}

			//Check if HDR is enabled
			BOOL hdrEnabled = vCaptureDetails.HDREnabled && !vCaptureDetails.HDRtoSDR;

			//Set encoder settings
			imfMediaTypeVideoOut->SetGUID(MF_MT_SUBTYPE, videoFormat);
			if (GetMonitorIsLimitedColorRange())
			{
				imfMediaTypeVideoOut->SetUINT32(MF_MT_VIDEO_NOMINAL_RANGE, MFNominalRange_16_235);
				AVDebugWriteLine("Set video nominal range to limited (16-235)");
			}
			else
			{
				imfMediaTypeVideoOut->SetUINT32(MF_MT_VIDEO_NOMINAL_RANGE, MFNominalRange_0_255);
				AVDebugWriteLine("Set video nominal range to full (0-255)");
			}

			if (videoFormat == MFVideoFormat_HEVC)
			{
				imfMediaTypeVideoOut->SetUINT32(MF_MT_VIDEO_LEVEL, eAVEncH265VLevel6_2);
				//HDR and SDR settings
				if (hdrEnabled)
				{
					imfMediaTypeVideoOut->SetUINT32(MF_MT_VIDEO_PROFILE, eAVEncH265VProfile_Main_420_10);
					imfMediaTypeVideoOut->SetUINT32(MF_MT_VIDEO_PRIMARIES, MFVideoPrimaries_BT2020);
					imfMediaTypeVideoOut->SetUINT32(MF_MT_YUV_MATRIX, MFVideoTransferMatrix_BT2020_10);
					imfMediaTypeVideoOut->SetUINT32(MF_MT_TRANSFER_FUNCTION, MFVideoTransFunc_2084); //PQ
					imfMediaTypeVideoOut->SetUINT32(MF_MT_MIN_MASTERING_LUMINANCE, 0);
					imfMediaTypeVideoOut->SetUINT32(MF_MT_MAX_MASTERING_LUMINANCE, vCaptureSettings.HDRMaximumNits);
					imfMediaTypeVideoOut->SetUINT32(MF_MT_MAX_LUMINANCE_LEVEL, vCaptureSettings.HDRMaximumNits);
					imfMediaTypeVideoOut->SetUINT32(MF_MT_MAX_FRAME_AVERAGE_LUMINANCE_LEVEL, vCaptureSettings.HDRMaximumNits);
				}
				else
				{
					imfMediaTypeVideoOut->SetUINT32(MF_MT_VIDEO_PROFILE, eAVEncH265VProfile_Main_420_8);
				}
			}
			else if (videoFormat == MFVideoFormat_AV1)
			{
				imfMediaTypeVideoOut->SetUINT32(MF_MT_VIDEO_LEVEL, eAVEncAV1VLevel6_3);
				//HDR and SDR settings
				if (hdrEnabled)
				{
					imfMediaTypeVideoOut->SetUINT32(MF_MT_VIDEO_PROFILE, eAVEncAV1VProfile_Main_420_10);
				}
				else
				{
					imfMediaTypeVideoOut->SetUINT32(MF_MT_VIDEO_PROFILE, eAVEncAV1VProfile_Main_420_8);
				}
			}
			else if (videoFormat == MFVideoFormat_H264)
			{
				imfMediaTypeVideoOut->SetUINT32(MF_MT_VIDEO_LEVEL, eAVEncH264VLevel5_2);
				imfMediaTypeVideoOut->SetUINT32(MF_MT_VIDEO_PROFILE, eAVEncH264VProfile_Main);
			}

			//Check monitor refresh rate
			UINT targetVideoFrameRate = vMediaSettings.VideoFrameRate;
			if (targetVideoFrameRate > vCaptureDetails.RefreshRate)
			{
				targetVideoFrameRate = vCaptureDetails.RefreshRate;
				AVDebugWriteLine("Monitor refresh rate lower, changed video fps to: " << targetVideoFrameRate);
			}
			MFSetAttributeRatio(imfMediaTypeVideoOut, MF_MT_FRAME_RATE, targetVideoFrameRate, 1);

			//Add video stream
			imfMediaTypeVideoOut->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
			imfMediaTypeVideoOut->SetUINT32(MF_MT_ALL_SAMPLES_INDEPENDENT, 1);
			imfMediaTypeVideoOut->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive);
			MFSetAttributeSize(imfMediaTypeVideoOut, MF_MT_FRAME_SIZE, vCaptureDetails.OutputWidth, vCaptureDetails.OutputHeight);
			MFSetAttributeRatio(imfMediaTypeVideoOut, MF_MT_PIXEL_ASPECT_RATIO, 1, 1);
			hResult = vMediaFoundationInstance.imfSinkWriter->AddStream(imfMediaTypeVideoOut, &vMediaFoundationInstance.vOutVideoStreamIndex);
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"AddStream video out failed") };
			}

			//Create video encoding parameters
			CComPtr<IMFAttributes> imfAttributesEncoding;
			MFCreateAttributes(&imfAttributesEncoding, 0);

			//Set video rate control
			imfAttributesEncoding->SetUINT32(CODECAPI_AVEncCommonMeanBitRate, vMediaSettings.VideoBitRate * 1000);
			if (vMediaSettings.VideoRateControl == CBR)
			{
				//Constant Rate Control
				AVDebugWriteLine("Set constant media control rate: " << vMediaSettings.VideoBitRate);
				imfAttributesEncoding->SetUINT32(CODECAPI_AVEncCommonRateControlMode, eAVEncCommonRateControlMode_CBR);
			}
			else if (vMediaSettings.VideoRateControl == VBR)
			{
				//Variable Rate Control
				AVDebugWriteLine("Set variable media control rate: " << vMediaSettings.VideoBitRate);
				imfAttributesEncoding->SetUINT32(CODECAPI_AVEncCommonRateControlMode, eAVEncCommonRateControlMode_UnconstrainedVBR);
			}

			//Set encoding settings
			imfAttributesEncoding->SetUINT32(CODECAPI_AVEncCommonQualityVsSpeed, 100);
			imfAttributesEncoding->SetUINT32(CODECAPI_AVEncCommonLowLatency, 1);
			imfAttributesEncoding->SetUINT32(CODECAPI_AVEncCommonRealTime, 1);
			imfAttributesEncoding->SetUINT32(CODECAPI_AVEncMPVGOPSize, targetVideoFrameRate * 2);
			imfAttributesEncoding->SetUINT32(CODECAPI_AVEncMPVDefaultBPictureCount, 0);

			//Create video in media type
			CComPtr<IMFMediaType> imfMediaTypeVideoIn;
			hResult = MFCreateMediaType(&imfMediaTypeVideoIn);
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"MFCreateMediaType video in failed") };
			}

			imfMediaTypeVideoIn->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
			imfMediaTypeVideoIn->SetUINT32(MF_MT_ALL_SAMPLES_INDEPENDENT, 1);
			MFSetAttributeSize(imfMediaTypeVideoIn, MF_MT_FRAME_SIZE, vCaptureDetails.OutputWidth, vCaptureDetails.OutputHeight);
			MFSetAttributeRatio(imfMediaTypeVideoIn, MF_MT_FRAME_RATE, vCaptureDetails.RefreshRate, 1);
			MFSetAttributeRatio(imfMediaTypeVideoIn, MF_MT_PIXEL_ASPECT_RATIO, 1, 1);
			//HDR and SDR settings
			if (hdrEnabled)
			{
				AVDebugWriteLine("Set media type format to HDR.");
				imfMediaTypeVideoIn->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_A16B16G16R16F);
			}
			else
			{
				AVDebugWriteLine("Set media type format to SDR.");
				imfMediaTypeVideoIn->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_ARGB32);
			}
			hResult = vMediaFoundationInstance.imfSinkWriter->SetInputMediaType(vMediaFoundationInstance.vOutVideoStreamIndex, imfMediaTypeVideoIn, imfAttributesEncoding);
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"SetInputMediaType video in failed") };
			}

			//Return result
			return { .Status = CaptureStatus::Success };
		}
		catch (...)
		{
			//Return result
			return { .Status = CaptureStatus::Failed, .Message = SysAllocString(L"SetVideoMediaType failed") };
		}
	}
}