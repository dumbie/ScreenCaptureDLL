#include "CaptureVariables.h"

namespace
{
	BYTE* Texture2DConvertToBitmapData(CComPtr<ID3D11Texture2D1>& textureTarget)
	{
		try
		{
			//Map texture to subresource
			D3D11_MAPPED_SUBRESOURCE iD3DMappedSubResource;
			hResult = iD3D11DeviceContext4->Map(textureTarget, 0, D3D11_MAP_READ, 0, &iD3DMappedSubResource);
			if (FAILED(hResult))
			{
				return NULL;
			}

			//Create image byte array
			BYTE* BitmapBuffer = new BYTE[vCaptureDetails.TotalByteSize];
			BYTE* BitmapBufferReturn = BitmapBuffer;
			BYTE* SourceBuffer = (BYTE*)iD3DMappedSubResource.pData;
			for (int i = 0; i < vCaptureDetails.Height; i++)
			{
				memcpy(BitmapBuffer, SourceBuffer, vCaptureDetails.WidthByteSize);
				SourceBuffer += iD3DMappedSubResource.RowPitch;
				BitmapBuffer += vCaptureDetails.WidthByteSize;
			}

			//Unmap texture from subresource
			iD3D11DeviceContext4->Unmap(textureTarget, 0);

			//Return image byte array
			return BitmapBufferReturn;
		}
		catch (...)
		{
			return NULL;
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

	BOOL Texture2DResizeMips(CComPtr<ID3D11Texture2D1>& textureTarget)
	{
		try
		{
			//Read texture description
			D3D11_TEXTURE2D_DESC1 iD3DTexture2D1DescResize{};
			textureTarget->GetDesc1(&iD3DTexture2D1DescResize);

			//Update texture description
			iD3DTexture2D1DescResize.MipLevels = vCaptureTextureMipLevels;
			iD3DTexture2D1DescResize.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

			//Create resize texture
			hResult = iD3D11Device5->CreateTexture2D1(&iD3DTexture2D1DescResize, NULL, &iD3D11Texture2D1Resize);
			if (FAILED(hResult))
			{
				return false;
			}

			//Copy target to resize texture
			iD3D11DeviceContext4->CopySubresourceRegion(iD3D11Texture2D1Resize, 0, 0, 0, 0, textureTarget, 0, NULL);

			//Create shader resource view
			hResult = iD3D11Device5->CreateShaderResourceView(iD3D11Texture2D1Resize, NULL, &iD3D11ShaderResourceView0Resize);
			if (FAILED(hResult))
			{
				return false;
			}

			//Generate texture mips to resize
			iD3D11DeviceContext4->GenerateMips(iD3D11ShaderResourceView0Resize);

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