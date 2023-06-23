#pragma once
#include "CaptureClasses.h"

namespace
{
	//Results
	HRESULT hResult;

	//Capture
	CaptureInstance vCaptureInstances[2];
	UINT vReferenceTimeFrameDuration = 100000;
	UINT vReferenceTimeToSeconds = 10000000;
	UINT vReferenceTimeToMilliseconds = 10000;
	UINT vReferenceTimeToMicroseconds = 1000;

	//Arrays
	FLOAT ColorRgbaBlack[] = { 0.0f, 0.0f, 0.0f, 0.0f };

	D3D_FEATURE_LEVEL D3DFeatureLevelsArray[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};
	UINT D3DFeatureLevelsCount = ARRAYSIZE(D3DFeatureLevelsArray);

	DXGI_FORMAT iDxgiFormatsArray[] =
	{
		DXGI_FORMAT_B8G8R8A8_UNORM,
		DXGI_FORMAT_R16G16B16A16_FLOAT
	};
	UINT iDxgiFormatsCount = ARRAYSIZE(iDxgiFormatsArray);

	D3D11_INPUT_ELEMENT_DESC InputElementsArray[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	UINT InputElementsCount = ARRAYSIZE(InputElementsArray);

	VertexVertice VertexVerticesArrayScreen[] =
	{
		{DirectX::XMFLOAT3(-1.0f, -1.0f, 0), DirectX::XMFLOAT2(0.0f, 1.0f)},
		{DirectX::XMFLOAT3(-1.0f, 1.0f, 0), DirectX::XMFLOAT2(0.0f, 0.0f)},
		{DirectX::XMFLOAT3(1.0f, -1.0f, 0), DirectX::XMFLOAT2(1.0f, 1.0f)},
		{DirectX::XMFLOAT3(1.0f, 1.0f, 0), DirectX::XMFLOAT2(1.0f, 0.0f)}
	};
	VertexVertice VertexVerticesArrayCursor[] =
	{
		{DirectX::XMFLOAT3(-1.0f, -1.0f, 0), DirectX::XMFLOAT2(0.0f, 1.0f)},
		{DirectX::XMFLOAT3(-1.0f, 1.0f, 0), DirectX::XMFLOAT2(0.0f, 0.0f)},
		{DirectX::XMFLOAT3(1.0f, -1.0f, 0), DirectX::XMFLOAT2(1.0f, 1.0f)},
		{DirectX::XMFLOAT3(1.0f, 1.0f, 0), DirectX::XMFLOAT2(1.0f, 0.0f)}
	};
	UINT VertexVerticesCount = ARRAYSIZE(VertexVerticesArrayScreen);
};