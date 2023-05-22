#pragma once
#include "CaptureVariables.h"

namespace
{
	CComPtr<ID3D11Texture2D1> Texture2DGetCurrent()
	{
		if (iD3D11Texture2D1Resize != NULL) { return iD3D11Texture2D1Resize; }
		if (iD3D11Texture2D1Cursor != NULL) { return iD3D11Texture2D1Cursor; }
		return iD3D11Texture2D1Capture;
	}

	SafeBytes Texture2DConvertToScreenBytes(CComPtr<ID3D11Texture2D1>& textureTarget, BOOL textureFlip)
	{
		try
		{
			//Map texture to subresource
			D3D11_MAPPED_SUBRESOURCE iD3DMappedSubResource;
			hResult = iD3D11DeviceContext4->Map(textureTarget, 0, D3D11_MAP_READ, 0, &iD3DMappedSubResource);
			if (FAILED(hResult))
			{
				return SafeBytes();
			}

			//Create image byte array
			SafeBytes BitmapBytes(vCaptureDetails.TotalByteSize);

			//Write image byte array
			BYTE* SourceBuffer = (BYTE*)iD3DMappedSubResource.pData;
			if (textureFlip)
			{
				BYTE* BitmapBuffer = BitmapBytes.Data + vCaptureDetails.TotalByteSize - vCaptureDetails.WidthByteSize;
				for (UINT i = 0; i < vCaptureDetails.Height; i++)
				{
					memcpy(BitmapBuffer, SourceBuffer, vCaptureDetails.WidthByteSize);
					SourceBuffer += iD3DMappedSubResource.RowPitch;
					BitmapBuffer -= vCaptureDetails.WidthByteSize;
				}
			}
			else
			{
				BYTE* BitmapBuffer = BitmapBytes.Data;
				for (UINT i = 0; i < vCaptureDetails.Height; i++)
				{
					memcpy(BitmapBuffer, SourceBuffer, vCaptureDetails.WidthByteSize);
					SourceBuffer += iD3DMappedSubResource.RowPitch;
					BitmapBuffer += vCaptureDetails.WidthByteSize;
				}
			}

			//Unmap texture from subresource
			iD3D11DeviceContext4->Unmap(textureTarget, 0);

			//Return result
			return BitmapBytes;
		}
		catch (...)
		{
			return SafeBytes();
		}
	}

	BOOL Texture2DConvertToCpuRead(CComPtr<ID3D11Texture2D1>& textureTarget)
	{
		try
		{
			//Read texture description
			D3D11_TEXTURE2D_DESC1 iD3DTexture2D1DescCpuRead{};
			textureTarget->GetDesc1(&iD3DTexture2D1DescCpuRead);

			//Update texture description
			iD3DTexture2D1DescCpuRead.Usage = D3D11_USAGE_STAGING;
			iD3DTexture2D1DescCpuRead.BindFlags = 0;
			iD3DTexture2D1DescCpuRead.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			iD3DTexture2D1DescCpuRead.MiscFlags = 0;

			//Create cpu texture
			hResult = iD3D11Device5->CreateTexture2D1(&iD3DTexture2D1DescCpuRead, NULL, &iD3D11Texture2D1CpuRead);
			if (FAILED(hResult))
			{
				return false;
			}

			//Copy target to cpu texture
			iD3D11DeviceContext4->CopySubresourceRegion(iD3D11Texture2D1CpuRead, 0, 0, 0, 0, textureTarget, 0, NULL);

			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	BOOL Texture2DDrawCursor(CComPtr<ID3D11Texture2D1>& textureTarget)
	{
		try
		{
			//Read texture description
			D3D11_TEXTURE2D_DESC1 iD3DTexture2D1DescCursor{};
			textureTarget->GetDesc1(&iD3DTexture2D1DescCursor);

			//Check texture format
			if (iD3DTexture2D1DescCursor.Format != DXGI_FORMAT_B8G8R8A8_UNORM && iD3DTexture2D1DescCursor.Format != DXGI_FORMAT_B8G8R8A8_UNORM_SRGB)
			{
				//std::cout << "Cursor can't be drawn on this texture format." << std::endl;
				return false;
			}

			//Update texture description
			iD3DTexture2D1DescCursor.Usage = D3D11_USAGE_DEFAULT;
			iD3DTexture2D1DescCursor.BindFlags = D3D11_BIND_RENDER_TARGET;
			iD3DTexture2D1DescCursor.CPUAccessFlags = 0;
			iD3DTexture2D1DescCursor.MiscFlags = D3D11_RESOURCE_MISC_GDI_COMPATIBLE;

			//Create cursor texture
			hResult = iD3D11Device5->CreateTexture2D1(&iD3DTexture2D1DescCursor, NULL, &iD3D11Texture2D1Cursor);
			if (FAILED(hResult))
			{
				return false;
			}

			//Copy target to cursor texture
			iD3D11DeviceContext4->CopySubresourceRegion(iD3D11Texture2D1Cursor, 0, 0, 0, 0, textureTarget, 0, NULL);

			//Convert variables
			hResult = iD3D11Texture2D1Cursor->QueryInterface(&iDxgiSurface2);
			if (FAILED(hResult))
			{
				return false;
			}

			//Draw cursor to texture surface
			CURSORINFO cursorInfo{};
			cursorInfo.cbSize = sizeof(cursorInfo);
			if (GetCursorInfo(&cursorInfo))
			{
				if (cursorInfo.flags == CURSOR_SHOWING)
				{
					HDC cursorHDC;
					POINT cursorPosition = cursorInfo.ptScreenPos;
					iDxgiSurface2->GetDC(FALSE, &cursorHDC);
					DrawIconEx(cursorHDC, cursorPosition.x, cursorPosition.y, cursorInfo.hCursor, 0, 0, 0, NULL, DI_NORMAL | DI_DEFAULTSIZE);
					iDxgiSurface2->ReleaseDC(NULL);
				}
			}

			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	BOOL Texture2DApplyShaders(CComPtr<ID3D11Texture2D1>& textureTarget)
	{
		try
		{
			//Create shader resource view
			hResult = iD3D11Device5->CreateShaderResourceView(textureTarget, NULL, &iD3D11ShaderResourceView0Shaders);
			if (FAILED(hResult))
			{
				return false;
			}

			//Set shader resource view
			iD3D11DeviceContext4->PSSetShaderResources(0, 1, &iD3D11ShaderResourceView0Shaders);

			//Draw texture with shaders
			iD3D11DeviceContext4->Draw(VertexVerticesCount, 0);

			return true;
		}
		catch (...)
		{
			return false;
		}
	}
};