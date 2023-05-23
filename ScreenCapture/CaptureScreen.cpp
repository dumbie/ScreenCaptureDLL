#pragma once
#include "CaptureVariables.h"
#include "CaptureInitialize.cpp"
#include "CaptureTexture.cpp"

namespace
{
	BOOL UpdateScreenTexture(BOOL waitNextFrame)
	{
		try
		{
			//Get output duplication frame
			UINT timeoutInMilliseconds = 0;
			if (waitNextFrame) { timeoutInMilliseconds = INFINITE; }
			DXGI_OUTDUPL_FRAME_INFO iDxgiOutputDuplicationFrameInfo;
			hResult = iDxgiOutputDuplication0->AcquireNextFrame(timeoutInMilliseconds, &iDxgiOutputDuplicationFrameInfo, &iDxgiResource0);

			//Check if screen captured
			if (FAILED(hResult))
			{
				//std::cout << "Failed to acquire next frame: " << hResult << std::endl;
				CaptureResetVariablesTexture();
				return false;
			}

			//Check mouse movement updates
			if (vCaptureSettings.MouseIgnoreMovement && iDxgiOutputDuplicationFrameInfo.LastPresentTime.QuadPart == 0)
			{
				//std::cout << "Acquire next frame, skipping mouse movement." << std::endl;
				CaptureResetVariablesTexture();
				return false;
			}

			//Convert variables
			hResult = iDxgiResource0->QueryInterface(&iD3D11Texture2D0Capture);
			if (FAILED(hResult))
			{
				CaptureResetVariablesTexture();
				return false;
			}

			//Apply shaders to texture
			if (!Texture2DApplyShaders(iD3D11Texture2D0Capture))
			{
				CaptureResetVariablesTexture();
				return false;
			}

			//Release resources
			CaptureResetVariablesTexture();

			//Return result
			return true;
		}
		catch (...)
		{
			std::cout << "UpdateScreenTexture failed." << std::endl;
			CaptureResetVariablesTexture();
			return false;
		}
	}

	std::vector<BYTE> GetScreenBytes(BOOL waitNextFrame, BOOL flipScreen)
	{
		try
		{
			//Update screen texture
			if (!UpdateScreenTexture(waitNextFrame))
			{
				CaptureResetVariablesTexture();
				return {};
			}

			//Convert to cpu read texture
			if (!Texture2DConvertToCpuRead(iD3D11Texture2D0RenderTargetView))
			{
				CaptureResetVariablesTexture();
				return {};
			}

			//Convert texture to screen bytes
			std::vector<BYTE> screenBytes = Texture2DConvertToScreenBytes(iD3D11Texture2D0CpuRead, flipScreen);

			//Release resources
			CaptureResetVariablesTexture();

			//Return result
			return screenBytes;
		}
		catch (...)
		{
			std::cout << "GetScreenBytes failed." << std::endl;
			CaptureResetVariablesTexture();
			return {};
		}
	}
}