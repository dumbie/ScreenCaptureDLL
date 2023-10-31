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
			hResult = vDirectXInstance.iDxgiOutputDuplication0->AcquireNextFrame(timeoutInMilliseconds, &iDxgiOutputDuplicationFrameInfo, &vCaptureInstance.iDxgiResource0);

			//Check if screen capture failed
			if (FAILED(hResult))
			{
				//std::cout << "Acquire next frame, failed: " << hResult << std::endl;
				CaptureResetVariablesTexturesLoop();
				return false;
			}

			//Check mouse movement updates
			if (vCaptureInstance.vCaptureSettings.MouseIgnoreMovement && iDxgiOutputDuplicationFrameInfo.LastPresentTime.QuadPart == 0)
			{
				//std::cout << "Acquire next frame, skipping mouse movement." << std::endl;
				CaptureResetVariablesTexturesLoop();
				return false;
			}

			//Draw screen capture to texture
			hResult = vCaptureInstance.iDxgiResource0->QueryInterface(&vCaptureInstance.iD3D11Texture2D0Screen);
			if (FAILED(hResult))
			{
				CaptureResetVariablesTexturesLoop();
				return false;
			}
			ResourceViewUpdateVertex(VertexVerticesArrayScreen);
			if (!ResourceViewDrawTexture2D(vCaptureInstance.iD3D11Texture2D0Screen))
			{
				CaptureResetVariablesTexturesLoop();
				return false;
			}

			//Draw mouse cursor to texture
			if (vCaptureInstance.vCaptureSettings.MouseDrawCursor)
			{
				UpdateMouseCursorVertex(iDxgiOutputDuplicationFrameInfo);
				UpdateMouseCursorTexture(iDxgiOutputDuplicationFrameInfo);
				if (vCaptureInstance.iD3D11Texture2D0Cursor != NULL)
				{
					ResourceViewUpdateVertex(VertexVerticesArrayCursor);
					ResourceViewDrawTexture2D(vCaptureInstance.iD3D11Texture2D0Cursor);
					//Fix do not use shaders when drawing mouse cursor
				}
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

	std::vector<BYTE> GetScreenBytes(BOOL waitNextFrame, BOOL flipScreen)
	{
		try
		{
			//Update screen texture
			if (!UpdateScreenTexture(waitNextFrame))
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