#pragma once
#include <directxmath.h>
#include <wincodec.h>

namespace
{
	typedef struct CaptureSettings
	{
		UINT MonitorId;
		UINT MaxPixelDimension;
		BOOL HDRtoSDR;
		FLOAT HDRBrightness;
		FLOAT Vibrance;
		FLOAT Saturate;
		FLOAT Temperature;
		FLOAT RedChannel;
		FLOAT GreenChannel;
		FLOAT BlueChannel;
		FLOAT Brightness;
		FLOAT Contrast;
		FLOAT Gamma;
	} CaptureSettings;

	typedef struct CaptureDetails
	{
		UINT Width;
		UINT Height;
		BOOL HDREnabled;
		FLOAT SDRWhiteLevel;
		UINT PixelByteSize;
		UINT WidthByteSize;
		UINT TotalByteSize;
	} CaptureDetails;

	typedef struct ShaderVariables
	{
		BOOL HDRtoSDR;
		FLOAT HDRBrightness;
		FLOAT SDRWhiteLevel;
		FLOAT Vibrance;
		FLOAT Saturate;
		FLOAT Temperature;
		FLOAT RedChannel;
		FLOAT GreenChannel;
		FLOAT BlueChannel;
		FLOAT Brightness;
		FLOAT Contrast;
		FLOAT Gamma;
	} ShaderVariables;

	typedef struct VertexVertice
	{
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT2 TexCoord;
	} VertexVertice;
};