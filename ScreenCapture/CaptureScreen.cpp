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
				//std::cout << "Frame is empty skipping convert." << std::endl;
				CaptureResetVariablesTexturesLoop();
				return false;
			}

			//Convert frame capture to texture
			auto access = frame.Surface().as<Windows::Graphics::DirectX::Direct3D11::IDirect3DDxgiInterfaceAccess>();
			hResult = access->GetInterface(winrt::guid_of<ID3D11Texture2D>(), (void**)&vCaptureInstance.iD3D11Texture2D0Screen);
			if (FAILED(hResult))
			{
				CaptureResetVariablesTexturesLoop();
				return false;
			}

			//Draw screen capture texture
			if (!RenderDrawTexture2D(vCaptureInstance.iD3D11Texture2D0Screen, VertexVerticesCountScreen))
			{
				CaptureResetVariablesTexturesLoop();
				return false;
			}

			//Release resources
			CaptureResetVariablesTexturesLoop();

			//Return result
			return true;
		}
		catch (...)
		{
			std::cout << "UpdateScreenTexture failed." << std::endl;
			CaptureResetVariablesTexturesLoop();
			return false;
		}
	}

	std::vector<BYTE> GetScreenBytes(BOOL flipScreen)
	{
		try
		{
			//Update screen texture
			if (!UpdateScreenTexture())
			{
				CaptureResetVariablesTexturesLoop();
				return {};
			}

			//Convert to cpu read texture
			if (!Texture2DConvertToCpuRead(vCaptureInstance.iD3D11Texture2D0RenderTargetView))
			{
				CaptureResetVariablesTexturesLoop();
				return {};
			}

			//Convert texture to screen bytes
			std::vector<BYTE> screenBytes = Texture2DConvertToScreenBytes(vCaptureInstance.iD3D11Texture2D0CpuRead, flipScreen);

			//Release resources
			CaptureResetVariablesTexturesLoop();

			//Return result
			return screenBytes;
		}
		catch (...)
		{
			std::cout << "GetScreenBytes failed." << std::endl;
			CaptureResetVariablesTexturesLoop();
			return {};
		}
	}
}