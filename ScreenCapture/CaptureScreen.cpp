#pragma once
#include "CaptureVariables.h"
#include "CaptureInitialize.cpp"
#include "CaptureTexture.cpp"

namespace
{
	BYTE* CaptureScreenBytes()
	{
		try
		{
			//Update SDR white level
			ULONGLONG currentTicks = GetTickCount64();
			if (vCaptureSettings.HDRtoSDR && (currentTicks - vCaptureSDRWhiteLevelTicks) > 3000)
			{
				UpdateMonitorSDRWhiteLevel();
				vCaptureSDRWhiteLevelTicks = currentTicks;
			}

			//Get output duplication frame
			DXGI_OUTDUPL_FRAME_INFO iDxgiOutputDuplicationFrameInfo;
			hResult = iDxgiOutputDuplication0->AcquireNextFrame(INFINITE, &iDxgiOutputDuplicationFrameInfo, &iDxgiResource0);
			if (FAILED(hResult))
			{
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

			//Check if texture is resizing
			if (vCaptureTextureResizing)
			{
				//Resize texture using mips
				if (!Texture2DResizeMips(iD3D11Texture2D1Capture))
				{
					CaptureResetVariablesTexture();
					return NULL;
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
					return NULL;
				}
			}

			//Convert to cpu read texture
			if (!Texture2DConvertToCpuRead(iD3D11Texture2D1RenderTargetView))
			{
				CaptureResetVariablesTexture();
				return NULL;
			}

			//Convert texture to bitmap data
			BYTE* bitmapData = Texture2DConvertToBitmapData(iD3D11Texture2D1CpuRead);

			//Release output duplication frame
			hResult = iDxgiOutputDuplication0->ReleaseFrame();
			if (FAILED(hResult))
			{
				CaptureResetVariablesTexture();
				return NULL;
			}

			//Release resources
			CaptureResetVariablesTexture();

			//Return bitmap data
			return bitmapData;
		}
		catch (...)
		{
			CaptureResetVariablesTexture();
			return NULL;
		}
	}
}