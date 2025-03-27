#pragma once
#include "CaptureIncludes.h"
#include "CaptureVariables.h"
#include "CaptureReset.cpp"
#include "CaptureTexture.cpp"
#include "CaptureRender.cpp"

namespace
{
	CaptureResult UpdateScreenTexture()
	{
		AVFinally(
			{
				//Release resources
				TextureResetVariablesLoop();
			});
		try
		{
			//Get frame from capture pool
			auto frame = vWgcInstance.vGraphicsCaptureFramePool.TryGetNextFrame();

			//Check if screen capture failed
			if (frame == NULL)
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"Frame is empty skipping convert") };
			}

			//Convert frame capture to texture
			auto access = frame.Surface().as<Windows::Graphics::DirectX::Direct3D11::IDirect3DDxgiInterfaceAccess>();
			hResult = access->GetInterface(winrt::guid_of<ID3D11Texture2D>(), (void**)&vCaptureInstance.iD3D11Texture2D0Screen);
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"Failed converting frame capture to texture") };
			}

			//Draw screen capture texture
			capResult = RenderDrawTexture2D(vCaptureInstance.iD3D11Texture2D0Screen, VertexVerticesCountScreen);
			if (capResult.Status != CaptureStatus::Success)
			{
				return capResult;
			}

			//Return result
			return { .Status = CaptureStatus::Success };
		}
		catch (...)
		{
			//Return result
			return { .Status = CaptureStatus::Failed, .Message = SysAllocString(L"UpdateScreenTexture failed") };
		}
	}

	std::vector<BYTE> GetScreenBytes(BOOL flipScreen)
	{
		AVFinally(
			{
				//Release resources
				TextureResetVariablesLoop();
			});
		try
		{
			//Check if initialized
			if (!vCaptureInstance.vInstanceInitialized)
			{
				return {};
			}

			//Update screen texture
			capResult = UpdateScreenTexture();
			if (capResult.Status != CaptureStatus::Success)
			{
				return {};
			}

			//Convert to cpu read texture
			capResult = Texture2DConvertToCpuRead(vCaptureInstance.iD3D11Texture2D0RenderTargetView);
			if (capResult.Status != CaptureStatus::Success)
			{
				return {};
			}

			//Convert texture to screen bytes
			std::vector<BYTE> screenBytes = Texture2DConvertToScreenBytes(vCaptureInstance.iD3D11Texture2D0CpuRead, flipScreen);

			//Return result
			return screenBytes;
		}
		catch (...)
		{
			//Return result
			AVDebugWriteLine("GetScreenBytes failed.");
			return {};
		}
	}
}