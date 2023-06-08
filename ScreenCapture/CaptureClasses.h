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

	enum VideoFormats
	{
		H264 = 0,
		HEVC = 1
	};

	enum AudioFormats
	{
		MP3 = 0,
		AAC = 1,
		FLAC = 2
	};

	//Structures
	struct MediaSettings
	{
		AudioFormats AudioFormat = AAC;
		UINT AudioChannels = 2;
		UINT AudioBitRate = 320;
		UINT AudioBitDepth = 16;
		UINT AudioSampleRate = 48000;
		VideoFormats VideoFormat = HEVC;
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