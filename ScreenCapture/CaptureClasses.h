#pragma once
#include <directxmath.h>
#include <wincodec.h>

namespace
{
	struct CaptureSettings
	{
		UINT MonitorId;
		UINT MaxPixelDimension;
		BOOL HDRtoSDR;
		FLOAT HDRBrightness;
		FLOAT Vibrance;
		FLOAT Saturation;
		FLOAT RedChannel;
		FLOAT GreenChannel;
		FLOAT BlueChannel;
		FLOAT Brightness;
		FLOAT Contrast;
		FLOAT Gamma;
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
		FLOAT HDRBrightness;
		FLOAT SDRWhiteLevel;
		FLOAT Vibrance;
		FLOAT Saturation;
		FLOAT RedChannel;
		FLOAT GreenChannel;
		FLOAT BlueChannel;
		FLOAT Brightness;
		FLOAT Contrast;
		FLOAT Gamma;
		FLOAT Reserved1;
		FLOAT Reserved2;
	};

	struct VertexVertice
	{
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT2 TexCoord;
	};
};