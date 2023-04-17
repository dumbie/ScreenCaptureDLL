#pragma once
#include "CaptureVariables.h"
#include "CaptureInitialize.cpp"
#include "CaptureTexture.cpp"

namespace
{
	BOOL UpdateScreenBytesCache(BOOL waitNextFrame, BOOL flipScreen)
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
			if (iDxgiOutputDuplicationFrameInfo.LastPresentTime.HighPart == 0)
			{
				//std::cout << "Acquire next frame, skipping mouse movement." << std::endl;
				CaptureResetVariablesTexture();
				return false;
			}

			//Convert variables
			hResult = iDxgiResource0->QueryInterface(&iD3D11Texture2D1Capture);
			if (FAILED(hResult))
			{
				CaptureResetVariablesTexture();
				return false;
			}

			////Draw cursor to texture
			//Texture2DDrawCursor(iD3D11Texture2D1Capture);

			//Check if texture is resizing
			if (vCaptureTextureResizing)
			{
				//Resize texture using mips
				if (!Texture2DResizeMips(iD3D11Texture2D1Capture))
				{
					CaptureResetVariablesTexture();
					return false;
				}

				//Apply shaders to texture
				if (!Texture2DApplyShaders(iD3D11Texture2D1Resize))
				{
					CaptureResetVariablesTexture();
					return false;
				}
			}
			else
			{
				//Apply shaders to texture
				if (!Texture2DApplyShaders(iD3D11Texture2D1Capture))
				{
					CaptureResetVariablesTexture();
					return false;
				}
			}

			//Convert to cpu read texture
			if (!Texture2DConvertToCpuRead(iD3D11Texture2D1RenderTargetView))
			{
				CaptureResetVariablesTexture();
				return false;
			}

			//Convert texture to screen bytes cache
			if (!Texture2DConvertToScreenBytesCache(iD3D11Texture2D1CpuRead, flipScreen))
			{
				CaptureResetVariablesTexture();
				return false;
			}

			//Release resources
			CaptureResetVariablesTexture();

			return true;
		}
		catch (...)
		{
			CaptureResetVariablesTexture();
			return false;
		}
	}
}