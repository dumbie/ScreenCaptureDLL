#pragma once
#include "CaptureIncludes.h"
#include "CaptureVariables.h"
#include "CaptureReset.cpp"
#include "CaptureTexture.cpp"
#include "CaptureRender.cpp"

namespace
{
	BOOL UpdateScreenTexture()
	{
		try
		{
			//Get frame from capture pool
			auto frame = vWgcInstance.vGraphicsCaptureFramePool.TryGetNextFrame();

			//Check if screen capture failed
			if (frame == NULL)
			{
				//AVDebugWriteLine("Frame is empty skipping convert.");
				TextureResetVariablesLoop();
				return false;
			}

			//Convert frame capture to texture
			auto access = frame.Surface().as<Windows::Graphics::DirectX::Direct3D11::IDirect3DDxgiInterfaceAccess>();
			hResult = access->GetInterface(winrt::guid_of<ID3D11Texture2D>(), (void**)&vCaptureInstance.iD3D11Texture2D0Screen);
			if (FAILED(hResult))
			{
				TextureResetVariablesLoop();
				return false;
			}

			//Draw screen capture texture
			if (!RenderDrawTexture2D(vCaptureInstance.iD3D11Texture2D0Screen, VertexVerticesCountScreen))
			{
				TextureResetVariablesLoop();
				return false;
			}

			//Release resources
			TextureResetVariablesLoop();

			//Return result
			return true;
		}
		catch (...)
		{
			AVDebugWriteLine("UpdateScreenTexture failed.");
			TextureResetVariablesLoop();
			return false;
		}
	}

	std::vector<BYTE> GetScreenBytes(BOOL flipScreen)
	{
		try
		{
			//Check if initialized
			if (!vCaptureInstance.vInstanceInitialized)
			{
				TextureResetVariablesLoop();
				return {};
			}

			//Update screen texture
			if (!UpdateScreenTexture())
			{
				TextureResetVariablesLoop();
				return {};
			}

			//Convert to cpu read texture
			if (!Texture2DConvertToCpuRead(vCaptureInstance.iD3D11Texture2D0RenderTargetView))
			{
				TextureResetVariablesLoop();
				return {};
			}

			//Convert texture to screen bytes
			std::vector<BYTE> screenBytes = Texture2DConvertToScreenBytes(vCaptureInstance.iD3D11Texture2D0CpuRead, flipScreen);

			//Release resources
			TextureResetVariablesLoop();

			//Return result
			return screenBytes;
		}
		catch (...)
		{
			AVDebugWriteLine("GetScreenBytes failed.");
			TextureResetVariablesLoop();
			return {};
		}
	}
}