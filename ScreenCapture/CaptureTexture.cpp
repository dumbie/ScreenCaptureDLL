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
			hResult = vDirectXInstance.iD3D11DeviceContext4->Map(textureTarget, 0, D3D11_MAP_READ, 0, &iD3DMappedSubResource);
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
				for (UINT i = 0; i < vCaptureDetails.OutputHeight; i++)
				{
					memcpy(BitmapBuffer, SourceBuffer, vCaptureDetails.WidthByteSize);
					SourceBuffer += iD3DMappedSubResource.RowPitch;
					BitmapBuffer -= vCaptureDetails.WidthByteSize;
				}
			}
			else
			{
				BYTE* BitmapBuffer = BitmapBytes.data();
				for (UINT i = 0; i < vCaptureDetails.OutputHeight; i++)
				{
					memcpy(BitmapBuffer, SourceBuffer, vCaptureDetails.WidthByteSize);
					SourceBuffer += iD3DMappedSubResource.RowPitch;
					BitmapBuffer += vCaptureDetails.WidthByteSize;
				}
			}

			//Unmap texture from subresource
			vDirectXInstance.iD3D11DeviceContext4->Unmap(textureTarget, 0);

			//Return result
			return BitmapBytes;
		}
		catch (...)
		{
			//Return result
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
			hResult = vDirectXInstance.iD3D11Device5->CreateTexture2D(&iD3DTexture2D0DescCpuRead, NULL, &vCaptureInstance.iD3D11Texture2D0CpuRead);
			if (FAILED(hResult))
			{
				return false;
			}

			//Copy target to cpu texture
			vDirectXInstance.iD3D11DeviceContext4->CopySubresourceRegion(vCaptureInstance.iD3D11Texture2D0CpuRead, 0, 0, 0, 0, textureTarget, 0, NULL);

			//Return result
			return true;
		}
		catch (...)
		{
			//Return result
			return false;
		}
	}
}