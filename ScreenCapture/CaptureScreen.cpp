#pragma once
#include "CaptureVariables.h"
#include "CaptureInitialize.cpp"
#include "CaptureTexture.cpp"

namespace
{
	DataBytes CaptureScreenBytes()
	{
		DataBytes dataBytes{};
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
				return dataBytes;
			}

			//Convert variables
			hResult = iDxgiResource0->QueryInterface(&iD3D11Texture2D1Capture);
			if (FAILED(hResult))
			{
				CaptureResetVariablesTexture();
				return dataBytes;
			}

			//Check if texture is resizing
			if (vCaptureTextureResizing)
			{
				//Resize texture using mips
				if (!Texture2DResizeMips(iD3D11Texture2D1Capture))
				{
					CaptureResetVariablesTexture();
					return dataBytes;
				}

				//Apply shaders to texture
				if (!Texture2DApplyShaders(iD3D11Texture2D1Resize))
				{
					CaptureResetVariablesTexture();
					return dataBytes;
				}
			}
			else
			{
				//Apply shaders to texture
				if (!Texture2DApplyShaders(iD3D11Texture2D1Capture))
				{
					CaptureResetVariablesTexture();
					return dataBytes;
				}
			}

			//Convert to cpu read texture
			if (!Texture2DConvertToCpuRead(iD3D11Texture2D1RenderTargetView))
			{
				CaptureResetVariablesTexture();
				return dataBytes;
			}

			//Convert texture to bitmap data
			BYTE* bitmapData = Texture2DConvertToBitmapData(iD3D11Texture2D1CpuRead);

			//Release output duplication frame
			hResult = iDxgiOutputDuplication0->ReleaseFrame();
			if (FAILED(hResult))
			{
				CaptureResetVariablesTexture();
				return dataBytes;
			}

			//Release resources
			CaptureResetVariablesTexture();

			//Return bitmap data
			dataBytes.Bytes = bitmapData;
			dataBytes.Size = vCaptureDetails.TotalByteSize;
			return dataBytes;
		}
		catch (...)
		{
			CaptureResetVariablesTexture();
			return dataBytes;
		}
	}
}