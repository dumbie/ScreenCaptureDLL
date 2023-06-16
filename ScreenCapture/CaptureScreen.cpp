#pragma once
#include "CaptureVariables.h"
#include "CaptureInitialize.cpp"
#include "CaptureTexture.cpp"
#include "CaptureRender.cpp"
#include "CaptureCursor.cpp"

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

			//Check if screen capture failed
			if (FAILED(hResult))
			{
				//std::cout << "Acquire next frame, failed: " << hResult << std::endl;
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

			//Draw screen capture to texture
			hResult = iDxgiResource0->QueryInterface(&iD3D11Texture2D0Screen);
			if (FAILED(hResult))
			{
				CaptureResetVariablesTexture();
				return false;
			}
			ResourceViewUpdateVertex(VertexVerticesArrayScreen);
			if (!ResourceViewDrawTexture2D(iD3D11Texture2D0Screen))
			{
				CaptureResetVariablesTexture();
				return false;
			}

			//Draw mouse cursor to texture
			if (vCaptureSettings.MouseDrawCursor)
			{
				UpdateMouseCursorVertex(iDxgiOutputDuplicationFrameInfo);
				UpdateMouseCursorTexture(iDxgiOutputDuplicationFrameInfo);
				if (iD3D11Texture2D0Cursor != NULL)
				{
					ResourceViewUpdateVertex(VertexVerticesArrayCursor);
					ResourceViewDrawTexture2D(iD3D11Texture2D0Cursor);
				}
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
			if (!vMediaCapturing)
			{
				if (!UpdateScreenTexture(waitNextFrame))
				{
					CaptureResetVariablesTexture();
					return {};
				}
			}

			//Convert to cpu read texture
			if (!Texture2DConvertToCpuRead(iD3D11Texture2D0RenderTargetView))
			{
				if (!vMediaCapturing)
				{
					CaptureResetVariablesTexture();
					return {};
				}
			}

			//Convert texture to screen bytes
			std::vector<BYTE> screenBytes = Texture2DConvertToScreenBytes(iD3D11Texture2D0CpuRead, flipScreen);

			//Release resources
			if (!vMediaCapturing)
			{
				CaptureResetVariablesTexture();
			}

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