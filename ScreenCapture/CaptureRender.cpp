#pragma once
#include "CaptureVariables.h"

namespace
{
	BOOL ResourceViewUpdateVertex(VertexVertice* vertexVerticesArray)
	{
		try
		{
			//Create buffer description
			D3D11_BUFFER_DESC bufferDescription{};
			bufferDescription.ByteWidth = sizeof(VertexVertice) * VertexVerticesCount;
			bufferDescription.Usage = D3D11_USAGE_DEFAULT;
			bufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bufferDescription.CPUAccessFlags = 0;
			bufferDescription.MiscFlags = 0;
			bufferDescription.StructureByteStride = 0;

			//Create subresource data
			D3D11_SUBRESOURCE_DATA subResourceData{};
			subResourceData.pSysMem = vertexVerticesArray;

			//Create vertex buffer
			hResult = iD3D11Device5->CreateBuffer(&bufferDescription, &subResourceData, &iD3D11Buffer0);
			if (FAILED(hResult))
			{
				return false;
			}

			//Set vertex buffer
			UINT bufferOffsets = 0;
			UINT bufferStrides = sizeof(VertexVertice);
			iD3D11DeviceContext4->IASetVertexBuffers(0, 1, &iD3D11Buffer0, &bufferStrides, &bufferOffsets);
			iD3D11DeviceContext4->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

			//Return result
			return true;
		}
		catch (...)
		{
			std::cout << "ResourceViewUpdateVertex failed." << std::endl;
			return false;
		}
	}

	BOOL ResourceViewDrawTexture2D(CComPtr<ID3D11Texture2D>& textureTarget)
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
			std::cout << "ResourceViewDrawTexture2D failed." << std::endl;
			return false;
		}
	}
}