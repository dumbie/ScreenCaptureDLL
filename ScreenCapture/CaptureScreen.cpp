#pragma once
#include "CaptureVariables.h"
#include "CaptureInitialize.cpp"
#include "CaptureTexture.cpp"

namespace
{
	BYTE* GetScreenBytes(BOOL waitNextFrame, BOOL flipScreen)
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
				return NULL;
			}

			//Convert variables
			hResult = iDxgiResource0->QueryInterface(&iD3D11Texture2D1Capture);
			if (FAILED(hResult))
			{
				CaptureResetVariablesTexture();
				return NULL;
			}

			//Draw cursor to texture
			if (vCaptureSettings.DrawCursor)
			{
				Texture2DDrawCursor(Texture2DGetCurrent());
			}

			//Resize texture using mips
			if (vCaptureTextureResizing)
			{
				if (!Texture2DResizeMips(Texture2DGetCurrent()))
				{
					CaptureResetVariablesTexture();
					return NULL;
				}
			}

			//Apply shaders to texture
			if (!Texture2DApplyShaders(Texture2DGetCurrent()))
			{
				CaptureResetVariablesTexture();
				return NULL;
			}

			//Convert to cpu read texture
			if (!Texture2DConvertToCpuRead(iD3D11Texture2D1RenderTargetView))
			{
				CaptureResetVariablesTexture();
				return NULL;
			}

			//Convert texture to screen bytes
			BYTE* screenBytes = Texture2DConvertToScreenBytes(iD3D11Texture2D1CpuRead, flipScreen);

			//Release resources
			CaptureResetVariablesTexture();

			//Return result
			return screenBytes;
		}
		catch (...)
		{
			CaptureResetVariablesTexture();
			return NULL;
		}
	}
}