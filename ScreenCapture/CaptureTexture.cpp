#pragma once
#include "CaptureVariables.h"

namespace
{
	std::vector<BYTE> Texture2DConvertToScreenBytes(CComPtr<ID3D11Texture2D>& textureTarget, BOOL textureFlip)
	{
		try
		{
			//Map texture to subresource
			D3D11_MAPPED_SUBRESOURCE iD3DMappedSubResource;
			hResult = iD3D11DeviceContext4->Map(textureTarget, 0, D3D11_MAP_READ, 0, &iD3DMappedSubResource);
			if (FAILED(hResult))
			{
				return {};
			}

			//Create image byte array
			std::vector<BYTE> BitmapBytes(vCaptureDetails.TotalByteSize);

			//Write image byte array
			BYTE* SourceBuffer = (BYTE*)iD3DMappedSubResource.pData;
			if (textureFlip)
			{
				BYTE* BitmapBuffer = BitmapBytes.data() + vCaptureDetails.TotalByteSize - vCaptureDetails.WidthByteSize;
				for (UINT i = 0; i < vCaptureDetails.Height; i++)
				{
					memcpy(BitmapBuffer, SourceBuffer, vCaptureDetails.WidthByteSize);
					SourceBuffer += iD3DMappedSubResource.RowPitch;
					BitmapBuffer -= vCaptureDetails.WidthByteSize;
				}
			}
			else
			{
				BYTE* BitmapBuffer = BitmapBytes.data();
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
			return {};
		}
	}

	BOOL Texture2DConvertToCpuRead(CComPtr<ID3D11Texture2D>& textureTarget)
	{
		try
		{
			//Read texture description
			D3D11_TEXTURE2D_DESC iD3DTexture2D0DescCpuRead{};
			textureTarget->GetDesc(&iD3DTexture2D0DescCpuRead);

			//Update texture description
			iD3DTexture2D0DescCpuRead.Usage = D3D11_USAGE_STAGING;
			iD3DTexture2D0DescCpuRead.BindFlags = 0;
			iD3DTexture2D0DescCpuRead.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			iD3DTexture2D0DescCpuRead.MiscFlags = 0;

			//Create cpu texture
			hResult = iD3D11Device5->CreateTexture2D(&iD3DTexture2D0DescCpuRead, NULL, &iD3D11Texture2D0CpuRead);
			if (FAILED(hResult))
			{
				return false;
			}

			//Copy target to cpu texture
			iD3D11DeviceContext4->CopySubresourceRegion(iD3D11Texture2D0CpuRead, 0, 0, 0, 0, textureTarget, 0, NULL);

			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	BOOL Texture2DDrawCursor(CComPtr<ID3D11Texture2D>& textureTarget)
	{
		try
		{
			//Read texture description
			D3D11_TEXTURE2D_DESC iD3DTexture2D0DescCursor{};
			textureTarget->GetDesc(&iD3DTexture2D0DescCursor);

			//Check texture format
			if (iD3DTexture2D0DescCursor.Format != DXGI_FORMAT_B8G8R8A8_UNORM && iD3DTexture2D0DescCursor.Format != DXGI_FORMAT_B8G8R8A8_UNORM_SRGB)
			{
				//std::cout << "Cursor can't be drawn on this texture format." << std::endl;
				return false;
			}

			//Update texture description
			iD3DTexture2D0DescCursor.MiscFlags = D3D11_RESOURCE_MISC_GDI_COMPATIBLE;

			//Create cursor texture
			hResult = iD3D11Device5->CreateTexture2D(&iD3DTexture2D0DescCursor, NULL, &iD3D11Texture2D0Cursor);
			if (FAILED(hResult))
			{
				return false;
			}

			//Copy target to cursor texture
			iD3D11DeviceContext4->CopySubresourceRegion(iD3D11Texture2D0Cursor, 0, 0, 0, 0, textureTarget, 0, NULL);

			//Convert variables
			hResult = iD3D11Texture2D0Cursor->QueryInterface(&iDxgiSurface2);
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

	BOOL Texture2DApplyShaders(CComPtr<ID3D11Texture2D>& textureTarget)
	{
		try
		{
			//Create shader resource view
			hResult = iD3D11Device5->CreateShaderResourceView(textureTarget, NULL, &iD3D11ShaderResourceView0);
			if (FAILED(hResult))
			{
				return false;
			}

			//Set shader resource view
			iD3D11DeviceContext4->PSSetShaderResources(0, 1, &iD3D11ShaderResourceView0);

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