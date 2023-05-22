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

	//Structures
	struct SafeBytes
	{
		//Variables
		BYTE* Data = NULL;
		size_t Size = 0;

		//Create
		SafeBytes() {}
		SafeBytes(size_t bytesSize)
		{
			Data = new BYTE[bytesSize];
			Size = bytesSize;
		}

		//Check
		BOOL IsEmpty()
		{
			return Data == NULL;
		}

		//Copy
		BOOL CopyTo(SafeBytes& destination)
		{
			try
			{
				destination.Release();
				destination.Data = new BYTE[Size];
				destination.Size = Size;
				memcpy(destination.Data, Data, Size);
				return true;
			}
			catch (...)
			{
				return false;
			}
		}

		//Release
		BOOL Release()
		{
			try
			{
				if (Data != NULL)
				{
					delete[] Data;
					Data = NULL;
					Size = 0;
				}
				return true;
			}
			catch (...)
			{
				return false;
			}
		}
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
		BOOL MouseDrawCursor = false;
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
		UINT Width;
		UINT Height;
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