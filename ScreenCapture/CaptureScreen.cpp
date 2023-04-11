#pragma once
#include "CaptureVariables.h"
#include "CaptureInitialize.cpp"
#include "CaptureTexture.cpp"

namespace
{
	BOOL UpdateScreenBytesCache(BOOL waitNextFrame)
	{
		try
		{
			//Get output duplication frame
			UINT timeoutInMilliseconds = 0;
			if (waitNextFrame) { timeoutInMilliseconds = INFINITE; }
			DXGI_OUTDUPL_FRAME_INFO iDxgiOutputDuplicationFrameInfo;
			hResult = iDxgiOutputDuplication0->AcquireNextFrame(timeoutInMilliseconds, &iDxgiOutputDuplicationFrameInfo, &iDxgiResource0);
			if (FAILED(hResult))
			{
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
					return NULL;
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
				return NULL;
			}

			//Convert texture to bitmap bytes
			BYTE* bitmapBytes = Texture2DConvertToBitmapBytes(iD3D11Texture2D1CpuRead);

			//Release output duplication frame
			hResult = iDxgiOutputDuplication0->ReleaseFrame();
			if (FAILED(hResult))
			{
				CaptureResetVariablesTexture();
				return false;
			}

			//Release resources
			CaptureResetVariablesTexture();

			//Update bitmap bytes cache
			if (bitmapBytes != NULL)
			{
				//Release cached bytes
				free(vScreenBytesCache);

				//Update cached bytes
				vScreenBytesCache = bitmapBytes;
			}

			return true;
		}
		catch (...)
		{
			CaptureResetVariablesTexture();
			return false;
		}
	}
}