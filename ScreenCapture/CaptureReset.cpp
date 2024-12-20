#pragma once
#include "CaptureVariables.h"

namespace
{
	BOOL TextureResetVariablesLoop()
	{
		try
		{
			//Textures
			vCaptureInstance.iD3D11Texture2D0CpuRead.Release();
			vCaptureInstance.iD3D11Texture2D0Screen.Release();

			//Views
			vDirectXInstance.iD3D11ShaderResourceView0.Release();

			//AVDebugWriteLine("Reset loop Texture variables.");
			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	BOOL CaptureStatusLoopStop()
	{
		try
		{
			if (vCaptureInstance.vCaptureStatusLoopAllowed)
			{
				//Status
				vCaptureInstance.vCaptureStatusLoopAllowed = false;

				//Wait for loop to finish
				while (!vCaptureInstance.vCaptureStatusLoopFinished)
				{
					AVDebugWriteLine("Waiting for capture status loop to stop...");
					AVHighResDelay(100);
				}

				AVDebugWriteLine("Capture status loop has stopped.");
			}

			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	BOOL CaptureInstanceResetVariablesAll()
	{
		try
		{
			//Stop capture status loop
			CaptureStatusLoopStop();

			//Bytes
			vCaptureInstance.vScreenBytesCache.clear();

			//Textures
			vCaptureInstance.iD3D11Texture2D0CpuRead.Release();
			vCaptureInstance.iD3D11Texture2D0Screen.Release();
			vCaptureInstance.iD3D11Texture2D0RenderTargetView.Release();

			//Status
			vCaptureInstance.vInstanceInitialized = false;
			vCaptureInstance.vInstanceInitializing = false;

			AVDebugWriteLine("Reset all Capture instance variables.");
			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	BOOL BitmapImageResetVariablesAll()
	{
		try
		{
			//Bitmap image
			vBitmapImageInstance.iWicPixelFormatGuidSource = GUID_WICPixelFormatDontCare;
			vBitmapImageInstance.iPropertyBag2.Release();
			vBitmapImageInstance.iWICImagingFactory.Release();
			vBitmapImageInstance.iWICStream.Release();
			vBitmapImageInstance.iWICBitmapEncoder.Release();
			vBitmapImageInstance.iWICBitmapFrameEncode.Release();
			vBitmapImageInstance.iWICMetadataQueryWriter.Release();
			vBitmapImageInstance.iWICFormatConverter.Release();
			vBitmapImageInstance.iWICBitmap.Release();

			AVDebugWriteLine("Reset all Bitmap Image variables.");
			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	BOOL MediaFoundationLoopStop()
	{
		try
		{
			if (vMediaFoundationInstance.vMediaWriteLoopAllowed)
			{
				//Status
				vMediaFoundationInstance.vMediaWriteLoopAllowed = false;

				//Wait for loop to finish
				while (!vMediaFoundationInstance.vMediaWriteLoopFinishedScreen && !vMediaFoundationInstance.vMediaWriteLoopFinishedAudio)
				{
					AVDebugWriteLine("Waiting for capture media loop to stop...");
					AVHighResDelay(100);
				}

				AVDebugWriteLine("Capture media loop has stopped.");
			}

			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	BOOL MediaFoundationResetVariablesAll()
	{
		try
		{
			//Stop media capture loop
			MediaFoundationLoopStop();

			//Media foundation
			vMediaFoundationInstance.imfSinkWriter.Release();
			vMediaFoundationInstance.imfDXGIDeviceManager.Release();

			//Device audio
			vMediaFoundationInstance.iDevice.Release();
			vMediaFoundationInstance.iAudioDeviceCapture.Release();
			vMediaFoundationInstance.iAudioDeviceRender.Release();
			vMediaFoundationInstance.iAudioClientCapture.Release();
			vMediaFoundationInstance.iAudioClientRender.Release();
			vMediaFoundationInstance.iAudioWaveFormatExRender.Free();

			//Status
			vMediaFoundationInstance.vMediaCapturing = false;

			AVDebugWriteLine("Reset all Media Foundation variables.");
			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	BOOL WgcResetVariablesAll()
	{
		try
		{
			//Frame
			vWgcInstance.vFrameSizeCurrent = { 0 ,0 };
			vWgcInstance.vFramePixelFormat = winrt::Windows::Graphics::DirectX::DirectXPixelFormat::Unknown;

			//Direct3D
			if (vWgcInstance.vGraphicsD3D11Device)
			{
				vWgcInstance.vGraphicsD3D11Device.Close();
			}
			vWgcInstance.vGraphicsD3D11Device = NULL;

			//Capture
			vWgcInstance.vGraphicsCaptureItem = NULL;
			if (vWgcInstance.vGraphicsCaptureSession)
			{
				vWgcInstance.vGraphicsCaptureSession.Close();
			}
			vWgcInstance.vGraphicsCaptureSession = NULL;
			if (vWgcInstance.vGraphicsCaptureFramePool)
			{
				vWgcInstance.vGraphicsCaptureFramePool.Close();
			}
			vWgcInstance.vGraphicsCaptureFramePool = NULL;

			AVDebugWriteLine("Reset all Windows Graphics Capture variables.");
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
			//Devices
			vDirectXInstance.iDxgiDevice4.Release();
			vDirectXInstance.iDxgiAdapter4.Release();
			vDirectXInstance.iDxgiFactory7.Release();
			vDirectXInstance.iDxgiOutput6.Release();
			vDirectXInstance.iD3D11Device5.Release();
			vDirectXInstance.iD3D11Multithread0.Release();
			vDirectXInstance.iD3D11DeviceContext4.Release();

			//States
			vDirectXInstance.iD3D11SamplerState0.Release();

			//Views
			vDirectXInstance.iD3D11InputLayout0.Release();
			vDirectXInstance.iD3D11RenderTargetView0.Release();
			vDirectXInstance.iD3D11ShaderResourceView0.Release();

			//Shaders
			vDirectXInstance.iD3D11BufferVertex0.Release();
			vDirectXInstance.iD3D11BufferPixel0.Release();
			vDirectXInstance.iD3D11ShaderVertex0.Release();
			vDirectXInstance.iD3D11ShaderPixel0.Release();

			AVDebugWriteLine("Reset all DirectX variables.");
			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	BOOL ResetVariablesAll()
	{
		try
		{
			//Capture Instance
			CaptureInstanceResetVariablesAll();

			//Windows Graphics Capture
			WgcResetVariablesAll();

			//DirectX
			DirectXResetVariablesAll();

			//Media foundation
			MediaFoundationResetVariablesAll();

			//Bitmap image
			BitmapImageResetVariablesAll();

			return true;
		}
		catch (...)
		{
			return false;
		}
	}
};