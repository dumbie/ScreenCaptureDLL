#pragma once
#include "CaptureVariables.h"

namespace
{
	std::vector<BYTE> Texture2DConvertToScreenBytes(UINT captureInstanceId, CComPtr<ID3D11Texture2D>& textureTarget, BOOL textureFlip)
	{
		try
		{
			//Map texture to subresource
			D3D11_MAPPED_SUBRESOURCE iD3DMappedSubResource;
			hResult = vCaptureInstances[captureInstanceId].iD3D11DeviceContext4->Map(textureTarget, 0, D3D11_MAP_READ, 0, &iD3DMappedSubResource);
			if (FAILED(hResult))
			{
				return {};
			}

			//Create image byte array
			std::vector<BYTE> BitmapBytes(vCaptureInstances[captureInstanceId].vCaptureDetails.TotalByteSize);

			//Write image byte array
			BYTE* SourceBuffer = (BYTE*)iD3DMappedSubResource.pData;
			if (textureFlip)
			{
				BYTE* BitmapBuffer = BitmapBytes.data() + vCaptureInstances[captureInstanceId].vCaptureDetails.TotalByteSize - vCaptureInstances[captureInstanceId].vCaptureDetails.WidthByteSize;
				for (UINT i = 0; i < vCaptureInstances[captureInstanceId].vCaptureDetails.OutputHeight; i++)
				{
					memcpy(BitmapBuffer, SourceBuffer, vCaptureInstances[captureInstanceId].vCaptureDetails.WidthByteSize);
					SourceBuffer += iD3DMappedSubResource.RowPitch;
					BitmapBuffer -= vCaptureInstances[captureInstanceId].vCaptureDetails.WidthByteSize;
				}
			}
			else
			{
				BYTE* BitmapBuffer = BitmapBytes.data();
				for (UINT i = 0; i < vCaptureInstances[captureInstanceId].vCaptureDetails.OutputHeight; i++)
				{
					memcpy(BitmapBuffer, SourceBuffer, vCaptureInstances[captureInstanceId].vCaptureDetails.WidthByteSize);
					SourceBuffer += iD3DMappedSubResource.RowPitch;
					BitmapBuffer += vCaptureInstances[captureInstanceId].vCaptureDetails.WidthByteSize;
				}
			}

			//Unmap texture from subresource
			vCaptureInstances[captureInstanceId].iD3D11DeviceContext4->Unmap(textureTarget, 0);

			//Return result
			return BitmapBytes;
		}
		catch (...)
		{
			return {};
		}
	}

	BOOL Texture2DConvertToCpuRead(UINT captureInstanceId, CComPtr<ID3D11Texture2D>& textureTarget)
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
			hResult = vCaptureInstances[captureInstanceId].iD3D11Device5->CreateTexture2D(&iD3DTexture2D0DescCpuRead, NULL, &vCaptureInstances[captureInstanceId].iD3D11Texture2D0CpuRead);
			if (FAILED(hResult))
			{
				return false;
			}

			//Copy target to cpu texture
			vCaptureInstances[captureInstanceId].iD3D11DeviceContext4->CopySubresourceRegion(vCaptureInstances[captureInstanceId].iD3D11Texture2D0CpuRead, 0, 0, 0, 0, textureTarget, 0, NULL);

			return true;
		}
		catch (...)
		{
			return false;
		}
	}
}