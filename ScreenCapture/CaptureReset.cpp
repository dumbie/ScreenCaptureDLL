#pragma once
#include "CaptureVariables.h"

namespace
{
	BOOL CaptureResetVariablesTexture()
	{
		try
		{
			//Views
			iD3D11ShaderResourceView0.Release();

			//Textures
			iDxgiSurface2.Release();
			iDxgiResource0.Release();
			iD3D11Texture2D0CpuRead.Release();
			iD3D11Texture2D0Capture.Release();
			iD3D11Texture2D0Cursor.Release();

			//Release output duplication frame
			if (iDxgiOutputDuplication0 != NULL)
			{
				iDxgiOutputDuplication0->ReleaseFrame();
			}

			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	BOOL CaptureResetVariablesBitmapImage()
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
			//Media foundation
			vMediaCapturing = false;
			vMediaWriteLoopAllowed = false;
			imfSinkWriter.Release();
			imfDXGIDeviceManager.Release();

			//Audio
			iDevice.Release();
			iAudioDeviceCapture.Release();
			iAudioDeviceRender.Release();
			iAudioClientCapture.Release();
			iAudioClientRender.Release();
			iAudioWaveFormatExCapture.Free();
			iAudioWaveFormatExRender.Free();

			//Bytes
			vScreenBytesCache.clear();

			return true;
		}
		catch (...)
		{
			return false;
		}
	}

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
			iD3D11Multithread.Release();
			iD3D11DeviceContext0.Release();
			iD3D11DeviceContext4.Release();

			//Sampler
			iD3D11SamplerState0.Release();

			//Views
			iD3D11InputLayout0.Release();
			iD3D11RenderTargetView0.Release();
			iD3D11ShaderResourceView0.Release();

			//Shaders
			iD3D11Buffer0.Release();
			iD3DBlob0VertexShader.Release();
			iD3DBlob0PixelShader.Release();
			iD3D11VertexShader0.Release();
			iD3D11PixelShader0.Release();

			//Textures
			iDxgiSurface2.Release();
			iDxgiResource0.Release();
			iD3D11Texture2D0CpuRead.Release();
			iD3D11Texture2D0Capture.Release();
			iD3D11Texture2D0Cursor.Release();
			iD3D11Texture2D0RenderTargetView.Release();

			//Bitmap
			CaptureResetVariablesBitmapImage();

			//Media
			CaptureResetVariablesMedia();

			std::cout << "Reset all capture variables." << std::endl;

			return true;
		}
		catch (...)
		{
			return false;
		}
	}
};