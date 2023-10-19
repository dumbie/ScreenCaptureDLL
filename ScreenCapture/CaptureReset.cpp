#pragma once
#include "CaptureVariables.h"

namespace
{
	BOOL CaptureResetVariablesTexturesLoop(UINT captureInstanceId)
	{
		try
		{
			//Textures
			vCaptureInstances[captureInstanceId].iDxgiResource0.Release();
			vCaptureInstances[captureInstanceId].iD3D11Texture2D0CpuRead.Release();
			vCaptureInstances[captureInstanceId].iD3D11Texture2D0Screen.Release();

			//Views
			vDirectXInstance.iD3D11ShaderResourceView0.Release();

			//Release output duplication frame
			if (vDirectXInstance.iDxgiOutputDuplication0 != NULL)
			{
				vDirectXInstance.iDxgiOutputDuplication0->ReleaseFrame();
			}

			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	BOOL CaptureResetVariablesTexturesAll(UINT captureInstanceId)
	{
		try
		{
			//Textures
			vCaptureInstances[captureInstanceId].iDxgiResource0.Release();
			vCaptureInstances[captureInstanceId].iD3D11Texture2D0CpuRead.Release();
			vCaptureInstances[captureInstanceId].iD3D11Texture2D0Screen.Release();
			vCaptureInstances[captureInstanceId].iD3D11Texture2D0Cursor.Release();
			vCaptureInstances[captureInstanceId].iD3D11Texture2D0RenderTargetView.Release();

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

	BOOL DirectXResetVariablesAll()
	{
		try
		{
			//Status
			vDirectXInstance.vInstanceInitialized = false;

			//Devices
			vDirectXInstance.iDxgiDevice4.Release();
			vDirectXInstance.iDxgiAdapter4.Release();
			vDirectXInstance.iDxgiOutput0.Release();
			vDirectXInstance.iDxgiOutput6.Release();
			vDirectXInstance.iDxgiOutputDuplication0.Release();
			vDirectXInstance.iD3D11Device0.Release();
			vDirectXInstance.iD3D11Device5.Release();
			vDirectXInstance.iD3D11Multithread.Release();
			vDirectXInstance.iD3D11DeviceContext0.Release();
			vDirectXInstance.iD3D11DeviceContext4.Release();

			//States
			vDirectXInstance.iD3D11SamplerState0.Release();
			vDirectXInstance.iD3D11BlendState0.Release();

			//Views
			vDirectXInstance.iD3D11InputLayout0.Release();
			vDirectXInstance.iD3D11RenderTargetView0.Release();
			vDirectXInstance.iD3D11ShaderResourceView0.Release();

			//Shaders
			vDirectXInstance.iD3D11Buffer0.Release();
			vDirectXInstance.iD3DBlob0VertexShader.Release();
			vDirectXInstance.iD3DBlob0PixelShader.Release();
			vDirectXInstance.iD3D11VertexShader0.Release();
			vDirectXInstance.iD3D11PixelShader0.Release();

			std::cout << "Reset all DirectX variables." << std::endl;
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
			//DirectX
			DirectXResetVariablesAll();

			//Status
			vCaptureInstances[captureInstanceId].vInstanceInitialized = false;

			//Textures
			CaptureResetVariablesTexturesAll(captureInstanceId);

			//Bitmap
			CaptureResetVariablesBitmapImage(captureInstanceId);

			//Media
			CaptureResetVariablesMedia(captureInstanceId);

			std::cout << "Reset all Capture variables for instance: " << captureInstanceId << std::endl;
			return true;
		}
		catch (...)
		{
			return false;
		}
	}
};