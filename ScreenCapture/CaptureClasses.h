#pragma once
#include "CaptureIncludes.h"

namespace
{
	//Enumerators
	enum ImageFormats
	{
		JXR = 0,
		JPG = 1,
		PNG = 2,
		BMP = 3,
		TIF = 4,
		HEIF = 5
	};

	struct MediaSettings
	{
		GUID AudioFormat = MFAudioFormat_AAC;
		UINT AudioChannels = 2;
		UINT AudioBitRate = 320;
		UINT AudioBits = 16;
		UINT AudioFrequency = 48000;
		GUID VideoFormat = MFVideoFormat_HEVC;
		UINT VideoQuality = 60;
		UINT VideoFrameRate = 60;
	};

	struct CaptureSettings
	{
		UINT MonitorId = 0;
		UINT MaxPixelDimension = 0;
		BOOL MouseIgnoreMovement = false;
		BOOL MouseDrawCursor = true;
		BOOL HDRtoSDR = false;
		FLOAT HDRPaperWhite = 80.0F;
		FLOAT HDRMaximumNits = 1000.0F;
		FLOAT Saturation = 1.0F;
		FLOAT RedChannel = 1.0F;
		FLOAT GreenChannel = 1.0F;
		FLOAT BlueChannel = 1.0F;
		FLOAT Brightness = 1.0F;
		FLOAT Contrast = 0.0F;
		FLOAT Gamma = 1.0F;
		FLOAT Blur = 0.0F;
	};

	struct CaptureDetails
	{
		UINT OriginalWidth;
		UINT OriginalHeight;
		UINT OutputWidth;
		UINT OutputHeight;
		UINT RefreshRate;
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