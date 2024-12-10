#pragma once
#include "CaptureClasses.h"

namespace
{
	//Results
	BOOL bResult;
	HRESULT hResult;
	NTSTATUS ntStatus;

	//Events
	typedef void(*CaptureEvent)();
	CaptureEvent vCaptureEventVideoCaptureStopped;
	CaptureEvent vCaptureEventDeviceChangeDetected;

	//Instances
	WgcInstance vWgcInstance;
	DirectXInstance vDirectXInstance;
	CaptureInstance vCaptureInstance;
	BitmapImageInstance vBitmapImageInstance;
	MediaFoundationInstance vMediaFoundationInstance;

	//Settings
	MediaSettings vMediaSettings;
	CaptureSettings vCaptureSettings;
	CaptureDetails vCaptureDetails;

	//Timings
	UINT vReferenceTimeToSeconds = 10000000;
	UINT vReferenceTimeToMilliseconds = 10000;
	UINT vReferenceTimeToMicroseconds = 1000;

	//Arrays
	FLOAT ColorRgbaBlack[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	FLOAT BlendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

	D3D_FEATURE_LEVEL D3DFeatureLevelsArray[] =
	{
		D3D_FEATURE_LEVEL_12_2,
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};
	UINT D3DFeatureLevelsCount = ARRAYSIZE(D3DFeatureLevelsArray);

	DXGI_FORMAT iDxgiFormatsArray[] =
	{
		DXGI_FORMAT_R16G16B16A16_FLOAT,
		DXGI_FORMAT_B8G8R8A8_UNORM
	};
	UINT iDxgiFormatsCount = ARRAYSIZE(iDxgiFormatsArray);

	D3DKMT_SCHEDULINGPRIORITYCLASS D3DKMTSchedulingPriorityClassArray[] =
	{
		D3DKMT_SCHEDULINGPRIORITYCLASS_REALTIME,
		D3DKMT_SCHEDULINGPRIORITYCLASS_HIGH
	};
	UINT D3DKMTSchedulingPriorityClassCount = ARRAYSIZE(D3DKMTSchedulingPriorityClassArray);

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
	UINT VertexVerticesCountScreen = ARRAYSIZE(VertexVerticesArrayScreen);
};