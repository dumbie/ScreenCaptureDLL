#pragma once
#include "CaptureVariables.h"
#include "CaptureInitialize.cpp"
#include "CaptureTexture.cpp"
#include "CaptureRender.cpp"
#include "CaptureCursor.cpp"

namespace
{
	BOOL UpdateScreenTexture(UINT captureInstanceId, BOOL waitNextFrame)
	{
		try
		{
			//Get output duplication frame
			UINT timeoutInMilliseconds = 0;
			if (waitNextFrame) { timeoutInMilliseconds = INFINITE; }
			DXGI_OUTDUPL_FRAME_INFO iDxgiOutputDuplicationFrameInfo;
			hResult = vCaptureInstances[captureInstanceId].iDxgiOutputDuplication0->AcquireNextFrame(timeoutInMilliseconds, &iDxgiOutputDuplicationFrameInfo, &vCaptureInstances[captureInstanceId].iDxgiResource0);

			//Check if screen capture failed
			if (FAILED(hResult))
			{
				//std::cout << "Acquire next frame, failed: " << hResult << std::endl;
				CaptureResetVariablesTexture(captureInstanceId);
				return false;
			}

			//Check mouse movement updates
			if (vCaptureInstances[captureInstanceId].vCaptureSettings.MouseIgnoreMovement && iDxgiOutputDuplicationFrameInfo.LastPresentTime.QuadPart == 0)
			{
				//std::cout << "Acquire next frame, skipping mouse movement." << std::endl;
				CaptureResetVariablesTexture(captureInstanceId);
				return false;
			}

			//Draw screen capture to texture
			hResult = vCaptureInstances[captureInstanceId].iDxgiResource0->QueryInterface(&vCaptureInstances[captureInstanceId].iD3D11Texture2D0Screen);
			if (FAILED(hResult))
			{
				CaptureResetVariablesTexture(captureInstanceId);
				return false;
			}
			ResourceViewUpdateVertex(captureInstanceId, VertexVerticesArrayScreen);
			if (!ResourceViewDrawTexture2D(captureInstanceId, vCaptureInstances[captureInstanceId].iD3D11Texture2D0Screen))
			{
				CaptureResetVariablesTexture(captureInstanceId);
				return false;
			}

			//Draw mouse cursor to texture
			if (vCaptureInstances[captureInstanceId].vCaptureSettings.MouseDrawCursor)
			{
				UpdateMouseCursorVertex(captureInstanceId, iDxgiOutputDuplicationFrameInfo);
				UpdateMouseCursorTexture(captureInstanceId, iDxgiOutputDuplicationFrameInfo);
				if (vCaptureInstances[captureInstanceId].iD3D11Texture2D0Cursor != NULL)
				{
					ResourceViewUpdateVertex(captureInstanceId, VertexVerticesArrayCursor);
					ResourceViewDrawTexture2D(captureInstanceId, vCaptureInstances[captureInstanceId].iD3D11Texture2D0Cursor);
				}
			}

			//Release resources
			CaptureResetVariablesTexture(captureInstanceId);

			//Return result
			return true;
		}
		catch (...)
		{
			std::cout << "UpdateScreenTexture failed." << std::endl;
			CaptureResetVariablesTexture(captureInstanceId);
			return false;
		}
	}

	std::vector<BYTE> GetScreenBytes(UINT captureInstanceId, BOOL waitNextFrame, BOOL flipScreen)
	{
		try
		{
			//Update screen texture
			if (!UpdateScreenTexture(captureInstanceId, waitNextFrame))
			{
				CaptureResetVariablesTexture(captureInstanceId);
				return {};
			}

			//Convert to cpu read texture
			if (!Texture2DConvertToCpuRead(captureInstanceId, vCaptureInstances[captureInstanceId].iD3D11Texture2D0RenderTargetView))
			{
				CaptureResetVariablesTexture(captureInstanceId);
				return {};
			}

			//Convert texture to screen bytes
			std::vector<BYTE> screenBytes = Texture2DConvertToScreenBytes(captureInstanceId, vCaptureInstances[captureInstanceId].iD3D11Texture2D0CpuRead, flipScreen);

			//Release resources
			CaptureResetVariablesTexture(captureInstanceId);

			//Return result
			return screenBytes;
		}
		catch (...)
		{
			std::cout << "GetScreenBytes failed." << std::endl;
			CaptureResetVariablesTexture(captureInstanceId);
			return {};
		}
	}
}