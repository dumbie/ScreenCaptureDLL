#pragma once
#include "CaptureVariables.h"

namespace
{
	BOOL CaptureResetVariablesTexture(UINT captureInstanceId)
	{
		try
		{
			//Views
			vCaptureInstances[captureInstanceId].vDirectXInstance.iD3D11ShaderResourceView0.Release();

			//Textures
			vCaptureInstances[captureInstanceId].iDxgiResource0.Release();
			vCaptureInstances[captureInstanceId].iD3D11Texture2D0CpuRead.Release();
			vCaptureInstances[captureInstanceId].iD3D11Texture2D0Screen.Release();

			//Release output duplication frame
			if (vCaptureInstances[captureInstanceId].vDirectXInstance.iDxgiOutputDuplication0 != NULL)
			{
				vCaptureInstances[captureInstanceId].vDirectXInstance.iDxgiOutputDuplication0->ReleaseFrame();
			}

			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	BOOL CaptureResetVariablesBitmapImage(UINT captureInstanceId)
	{
		try
		{
			//Bitmap
			vCaptureInstances[captureInstanceId].iPropertyBag2.Release();
			vCaptureInstances[captureInstanceId].iWICImagingFactory.Release();
			vCaptureInstances[captureInstanceId].iWICStream.Release();
			vCaptureInstances[captureInstanceId].iWICBitmapEncoder.Release();
			vCaptureInstances[captureInstanceId].iWICBitmapFrameEncode.Release();
			vCaptureInstances[captureInstanceId].iWICMetadataQueryWriter.Release();
			vCaptureInstances[captureInstanceId].iWICFormatConverter.Release();
			vCaptureInstances[captureInstanceId].iWICBitmap.Release();

			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	BOOL CaptureResetVariablesMedia(UINT captureInstanceId)
	{
		try
		{
			//Media foundation
			vCaptureInstances[captureInstanceId].vMediaCapturing = false;
			vCaptureInstances[captureInstanceId].vMediaWriteLoopAllowed = false;
			vCaptureInstances[captureInstanceId].imfSinkWriter.Release();
			vCaptureInstances[captureInstanceId].imfDXGIDeviceManager.Release();

			//Audio
			vCaptureInstances[captureInstanceId].iDevice.Release();
			vCaptureInstances[captureInstanceId].iAudioDeviceCapture.Release();
			vCaptureInstances[captureInstanceId].iAudioDeviceRender.Release();
			vCaptureInstances[captureInstanceId].iAudioClientCapture.Release();
			vCaptureInstances[captureInstanceId].iAudioClientRender.Release();
			vCaptureInstances[captureInstanceId].iAudioWaveFormatExCapture.Free();
			vCaptureInstances[captureInstanceId].iAudioWaveFormatExRender.Free();

			//Bytes
			vCaptureInstances[captureInstanceId].vScreenBytesCache.clear();

			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	BOOL CaptureResetVariablesAll(UINT captureInstanceId)
	{
		try
		{
			//Capture
			vCaptureInstances[captureInstanceId].vCaptureInstanceInitialized = false;

			//Devices
			vCaptureInstances[captureInstanceId].vDirectXInstance.iDxgiDevice4.Release();
			vCaptureInstances[captureInstanceId].vDirectXInstance.iDxgiAdapter4.Release();
			vCaptureInstances[captureInstanceId].vDirectXInstance.iDxgiOutput0.Release();
			vCaptureInstances[captureInstanceId].vDirectXInstance.iDxgiOutput6.Release();
			vCaptureInstances[captureInstanceId].vDirectXInstance.iDxgiOutputDuplication0.Release();
			vCaptureInstances[captureInstanceId].vDirectXInstance.iD3D11Device0.Release();
			vCaptureInstances[captureInstanceId].vDirectXInstance.iD3D11Device5.Release();
			vCaptureInstances[captureInstanceId].vDirectXInstance.iD3D11Multithread.Release();
			vCaptureInstances[captureInstanceId].vDirectXInstance.iD3D11DeviceContext0.Release();
			vCaptureInstances[captureInstanceId].vDirectXInstance.iD3D11DeviceContext4.Release();

			//States
			vCaptureInstances[captureInstanceId].vDirectXInstance.iD3D11SamplerState0.Release();
			vCaptureInstances[captureInstanceId].vDirectXInstance.iD3D11BlendState0.Release();

			//Views
			vCaptureInstances[captureInstanceId].vDirectXInstance.iD3D11InputLayout0.Release();
			vCaptureInstances[captureInstanceId].vDirectXInstance.iD3D11RenderTargetView0.Release();
			vCaptureInstances[captureInstanceId].vDirectXInstance.iD3D11ShaderResourceView0.Release();

			//Shaders
			vCaptureInstances[captureInstanceId].vDirectXInstance.iD3D11Buffer0.Release();
			vCaptureInstances[captureInstanceId].vDirectXInstance.iD3DBlob0VertexShader.Release();
			vCaptureInstances[captureInstanceId].vDirectXInstance.iD3DBlob0PixelShader.Release();
			vCaptureInstances[captureInstanceId].vDirectXInstance.iD3D11VertexShader0.Release();
			vCaptureInstances[captureInstanceId].vDirectXInstance.iD3D11PixelShader0.Release();

			//Textures
			vCaptureInstances[captureInstanceId].iDxgiResource0.Release();
			vCaptureInstances[captureInstanceId].iD3D11Texture2D0CpuRead.Release();
			vCaptureInstances[captureInstanceId].iD3D11Texture2D0Screen.Release();
			vCaptureInstances[captureInstanceId].iD3D11Texture2D0Cursor.Release();
			vCaptureInstances[captureInstanceId].iD3D11Texture2D0RenderTargetView.Release();

			//Bitmap
			CaptureResetVariablesBitmapImage(captureInstanceId);

			//Media
			CaptureResetVariablesMedia(captureInstanceId);

			std::cout << "Reset all capture variables for instance: " << captureInstanceId << std::endl;

			return true;
		}
		catch (...)
		{
			return false;
		}
	}
};