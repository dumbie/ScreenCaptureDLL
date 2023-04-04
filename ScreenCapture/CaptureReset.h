#pragma once
#include "CaptureVariables.h"

namespace
{
	BOOL CaptureResetVariablesAll()
	{
		try
		{
			//Devices
			iDxgiDevice4.Release();
			iDxgiAdapter4.Release();
			iDxgiOutput0.Release();
			iDxgiOutput6.Release();
			iDxgiOutputDuplication0.Release();
			iD3D11Device0.Release();
			iD3D11Device5.Release();
			iD3D11DeviceContext0.Release();
			iD3D11DeviceContext4.Release();

			//Sampler
			iD3D11SamplerState0.Release();

			//Views
			iD3D11InputLayout0.Release();
			iD3D11RenderTargetView0.Release();
			iD3D11ShaderResourceView0Shaders.Release();
			iD3D11ShaderResourceView0Resize.Release();

			//Shaders
			iD3D11Buffer0.Release();
			iD3DBlob0VertexShader.Release();
			iD3DBlob0PixelShader.Release();
			iD3D11VertexShader0.Release();
			iD3D11PixelShader0.Release();

			//Textures
			iDxgiResource0.Release();
			iD3D11Texture2D1CpuRead.Release();
			iD3D11Texture2D1Capture.Release();
			iD3D11Texture2D1Resize.Release();
			iD3D11Texture2D1RenderTargetView.Release();

			//Bitmap
			iPropertyBag2.Release();
			iWICImagingFactory.Release();
			iWICStream.Release();
			iWICBitmapEncoder.Release();
			iWICBitmapFrameEncode.Release();
			iWICMetadataQueryWriter.Release();
			iWICFormatConverter.Release();
			iWICBitmap.Release();

			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	BOOL CaptureResetVariablesTexture()
	{
		try
		{
			//Views
			iD3D11ShaderResourceView0Shaders.Release();
			iD3D11ShaderResourceView0Resize.Release();

			//Textures
			iDxgiResource0.Release();
			iD3D11Texture2D1CpuRead.Release();
			iD3D11Texture2D1Capture.Release();
			iD3D11Texture2D1Resize.Release();

			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	BOOL CaptureResetVariablesImage()
	{
		try
		{
			//Bitmap
			iPropertyBag2.Release();
			iWICImagingFactory.Release();
			iWICStream.Release();
			iWICBitmapEncoder.Release();
			iWICBitmapFrameEncode.Release();
			iWICMetadataQueryWriter.Release();
			iWICFormatConverter.Release();
			iWICBitmap.Release();

			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	BOOL CaptureResetVariablesMedia()
	{
		try
		{
			//Status
			vMediaCapturing = false;

			//CoTaskMemFree(iAudioWaveFormatEx);

			return true;
		}
		catch (...)
		{
			return false;
		}
	}
};