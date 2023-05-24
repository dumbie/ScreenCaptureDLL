#pragma once
#include "CaptureVariables.h"

namespace
{
	BOOL UpdateMouseCursorTexture(DXGI_OUTDUPL_FRAME_INFO cursorFrameInfo)
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
				hResult = iDxgiOutputDuplication0->GetFramePointerShape(cursorFrameInfo.PointerShapeBufferSize, pointerShapeBuffer.data(), &pointerShapeBufferSizeRequired, &pointerShapeInfo);
				if (FAILED(hResult))
				{
					return false;
				}

				//Update cursor variables
				vCursorWidth = pointerShapeInfo.Width;
				vCursorHeight = pointerShapeInfo.Height;

				//Create cursor texture description
				D3D11_TEXTURE2D_DESC iD3DTexture2D0DescCursor{};
				iD3DTexture2D0DescCursor.Width = vCursorWidth;
				iD3DTexture2D0DescCursor.Height = vCursorHeight;
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
				iD3D11Texture2D0Cursor.Release();

				//Create cursor texture
				hResult = iD3D11Device5->CreateTexture2D(&iD3DTexture2D0DescCursor, &subResourceDataCursor, &iD3D11Texture2D0Cursor);
				if (FAILED(hResult))
				{
					return false;
				}

				//std::cout << "Mouse cursor info, Size: " << pointerShapeBufferSizeRequired << " Height: " << pointerShapeInfo.Height << " Width: " << pointerShapeInfo.Width << std::endl;
				return true;
			}
		}
		catch (...)
		{
			std::cout << "UpdateMouseCursorTexture failed." << std::endl;
			return false;
		}
	}
}