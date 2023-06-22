#pragma once
#include "CaptureVariables.h"

namespace
{
	BOOL UpdateMouseCursorVertex(UINT captureInstanceId, DXGI_OUTDUPL_FRAME_INFO cursorFrameInfo)
	{
		try
		{
			if (cursorFrameInfo.PointerPosition.Visible)
			{
				//Update cursor vertex vertices
				INT cursorPositionLeft = cursorFrameInfo.PointerPosition.Position.x;
				INT cursorPositionTop = cursorFrameInfo.PointerPosition.Position.y;
				INT cursorWidth = vCaptureInstances[captureInstanceId].vCursorWidth;
				INT cursorHeight = vCaptureInstances[captureInstanceId].vCursorHeight;
				INT screenCenterX = vCaptureInstances[captureInstanceId].vCaptureDetails.OriginalWidth / 2;
				INT screenCenterY = vCaptureInstances[captureInstanceId].vCaptureDetails.OriginalHeight / 2;
				VertexVerticesArrayCursor[0].Position.x = (cursorPositionLeft - screenCenterX) / (FLOAT)screenCenterX;
				VertexVerticesArrayCursor[0].Position.y = -1 * ((cursorPositionTop + cursorHeight) - screenCenterY) / (FLOAT)screenCenterY;
				VertexVerticesArrayCursor[1].Position.x = (cursorPositionLeft - screenCenterX) / (FLOAT)screenCenterX;
				VertexVerticesArrayCursor[1].Position.y = -1 * (cursorPositionTop - screenCenterY) / (FLOAT)screenCenterY;
				VertexVerticesArrayCursor[2].Position.x = ((cursorPositionLeft + cursorWidth) - screenCenterX) / (FLOAT)screenCenterX;
				VertexVerticesArrayCursor[2].Position.y = -1 * ((cursorPositionTop + cursorHeight) - screenCenterY) / (FLOAT)screenCenterY;
				VertexVerticesArrayCursor[3].Position.x = ((cursorPositionLeft + cursorWidth) - screenCenterX) / (FLOAT)screenCenterX;
				VertexVerticesArrayCursor[3].Position.y = -1 * (cursorPositionTop - screenCenterY) / (FLOAT)screenCenterY;

				//std::cout << "Mouse cursor position, Left: " << cursorPositionLeft << " Top: " << cursorPositionTop << " Visible: " << cursorFrameInfo.PointerPosition.Visible << std::endl;
			}

			//Return result
			return true;
		}
		catch (...)
		{
			std::cout << "UpdateMouseCursorVertex failed." << std::endl;
			return false;
		}
	}

	BOOL UpdateMouseCursorTexture(UINT captureInstanceId, DXGI_OUTDUPL_FRAME_INFO cursorFrameInfo)
	{
		try
		{
			if (cursorFrameInfo.PointerShapeBufferSize > 0)
			{
				//Create cursor buffer
				std::vector<BYTE> pointerShapeBuffer;
				pointerShapeBuffer.resize(cursorFrameInfo.PointerShapeBufferSize);

				//Get cursor information
				UINT pointerShapeBufferSizeRequired;
				DXGI_OUTDUPL_POINTER_SHAPE_INFO pointerShapeInfo;
				hResult = vCaptureInstances[captureInstanceId].iDxgiOutputDuplication0->GetFramePointerShape(cursorFrameInfo.PointerShapeBufferSize, pointerShapeBuffer.data(), &pointerShapeBufferSizeRequired, &pointerShapeInfo);
				if (FAILED(hResult))
				{
					return false;
				}

				//Update cursor variables
				vCaptureInstances[captureInstanceId].vCursorWidth = pointerShapeInfo.Width;
				vCaptureInstances[captureInstanceId].vCursorHeight = pointerShapeInfo.Height;

				//Create cursor texture description
				D3D11_TEXTURE2D_DESC iD3DTexture2D0DescCursor{};
				iD3DTexture2D0DescCursor.Width = vCaptureInstances[captureInstanceId].vCursorWidth;
				iD3DTexture2D0DescCursor.Height = vCaptureInstances[captureInstanceId].vCursorHeight;
				iD3DTexture2D0DescCursor.MipLevels = 1;
				iD3DTexture2D0DescCursor.ArraySize = 1;
				iD3DTexture2D0DescCursor.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
				iD3DTexture2D0DescCursor.SampleDesc.Count = 1;
				iD3DTexture2D0DescCursor.SampleDesc.Quality = 0;
				iD3DTexture2D0DescCursor.Usage = D3D11_USAGE_DEFAULT;
				iD3DTexture2D0DescCursor.BindFlags = D3D11_BIND_SHADER_RESOURCE;
				iD3DTexture2D0DescCursor.CPUAccessFlags = 0;
				iD3DTexture2D0DescCursor.MiscFlags = 0;

				//Create subresource data
				D3D11_SUBRESOURCE_DATA subResourceDataCursor{};
				subResourceDataCursor.pSysMem = pointerShapeBuffer.data();
				subResourceDataCursor.SysMemPitch = iD3DTexture2D0DescCursor.Width * 4;
				subResourceDataCursor.SysMemSlicePitch = 0;

				//Release cursor texture
				vCaptureInstances[captureInstanceId].iD3D11Texture2D0Cursor.Release();

				//Create cursor texture
				hResult = vCaptureInstances[captureInstanceId].iD3D11Device5->CreateTexture2D(&iD3DTexture2D0DescCursor, &subResourceDataCursor, &vCaptureInstances[captureInstanceId].iD3D11Texture2D0Cursor);
				if (FAILED(hResult))
				{
					return false;
				}

				//std::cout << "Mouse cursor info, Size: " << pointerShapeBufferSizeRequired << " Height: " << pointerShapeInfo.Height << " Width: " << pointerShapeInfo.Width << std::endl;
			}

			//Return result
			return true;
		}
		catch (...)
		{
			std::cout << "UpdateMouseCursorTexture failed." << std::endl;
			return false;
		}
	}
}