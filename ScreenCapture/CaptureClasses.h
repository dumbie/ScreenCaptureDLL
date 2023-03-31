#pragma once
#include "CaptureIncludes.h"

namespace
{
	struct VideoSettings
	{
		WCHAR* FileName = L"E:\\ScreenCaptureTool.mp4";
		GUID AudioFormat = MFAudioFormat_MP3;
		UINT AudioChannels = 2;
		UINT AudioBitRate = 320;
		UINT AudioFrequency = 44100;
		GUID VideoFormat = MFVideoFormat_HEVC;
		UINT VideoBitRate = 6000;
		UINT VideoFrameRate = 60;
	};

	struct CaptureSettings
	{
		UINT MonitorId;
		UINT MaxPixelDimension;
		BOOL HDRtoSDR;
		FLOAT HDRPaperWhite;
		FLOAT HDRMaximumNits;
		FLOAT Saturation;
		FLOAT RedChannel;
		FLOAT GreenChannel;
		FLOAT BlueChannel;
		FLOAT Brightness;
		FLOAT Contrast;
		FLOAT Gamma;
		FLOAT Blur;
	};

	struct CaptureDetails
	{
		UINT Width;
		UINT Height;
		BOOL HDREnabled;
		FLOAT SDRWhiteLevel;
		UINT PixelByteSize;
		UINT WidthByteSize;
		UINT TotalByteSize;
	};

	struct ShaderVariables
	{
		BOOL HDRtoSDR;
		FLOAT HDRPaperWhite;
		FLOAT HDRMaximumNits;
		FLOAT SDRWhiteLevel;
		FLOAT Saturation;
		FLOAT RedChannel;
		FLOAT GreenChannel;
		FLOAT BlueChannel;
		FLOAT Brightness;
		FLOAT Contrast;
		FLOAT Gamma;
		FLOAT Blur;
	};

	struct VertexVertice
	{
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT2 TexCoord;
	};
};