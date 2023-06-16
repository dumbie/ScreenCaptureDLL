#pragma once
#include "CaptureClasses.h"

namespace
{
	//Results
	HRESULT hResult;

	//Capture
	CaptureSettings vCaptureSettings{};
	CaptureDetails vCaptureDetails{};
	BOOL vCaptureDeviceInitialized;
	BOOL vCaptureTextureResizing;
	UINT vCaptureTextureMipLevels;
	DXGI_FORMAT vCaptureDxgiFormat;

	//Device Direct3D
	CComPtr<IDXGIDevice4> iDxgiDevice4;
	CComPtr<IDXGIAdapter4> iDxgiAdapter4;
	CComPtr<IDXGIOutput> iDxgiOutput0;
	CComPtr<IDXGIOutput6> iDxgiOutput6;
	CComPtr<IDXGIOutputDuplication> iDxgiOutputDuplication0;
	CComPtr<ID3D11Device> iD3D11Device0;
	CComPtr<ID3D11Device5> iD3D11Device5;
	CComPtr<ID3D11Multithread> iD3D11Multithread;
	CComPtr<ID3D11DeviceContext> iD3D11DeviceContext0;
	CComPtr<ID3D11DeviceContext4> iD3D11DeviceContext4;

	//States
	CComPtr<ID3D11SamplerState> iD3D11SamplerState0;

	//Views
	CComPtr<ID3D11InputLayout> iD3D11InputLayout0;
	CComPtr<ID3D11RenderTargetView> iD3D11RenderTargetView0;
	CComPtr<ID3D11ShaderResourceView> iD3D11ShaderResourceView0;

	//Shaders
	CComPtr<ID3D11Buffer> iD3D11Buffer0;
	CComPtr<ID3DBlob> iD3DBlob0VertexShader;
	CComPtr<ID3DBlob> iD3DBlob0PixelShader;
	CComPtr<ID3D11VertexShader> iD3D11VertexShader0;
	CComPtr<ID3D11PixelShader> iD3D11PixelShader0;

	//Textures
	CComPtr<IDXGISurface2> iDxgiSurface2;
	CComPtr<IDXGIResource> iDxgiResource0;
	CComPtr<ID3D11Texture2D> iD3D11Texture2D0CpuRead;
	CComPtr<ID3D11Texture2D> iD3D11Texture2D0Screen;
	CComPtr<ID3D11Texture2D> iD3D11Texture2D0Cursor;
	CComPtr<ID3D11Texture2D> iD3D11Texture2D0RenderTargetView;

	//Image
	WICPixelFormatGUID iWicPixelFormatGuidSource;
	WICPixelFormatGUID iWicPixelFormatGuidJpeg = GUID_WICPixelFormat24bppBGR;
	CComPtr<IPropertyBag2> iPropertyBag2;
	CComPtr<IWICImagingFactory> iWICImagingFactory;
	CComPtr<IWICStream> iWICStream;
	CComPtr<IWICBitmapEncoder> iWICBitmapEncoder;
	CComPtr<IWICBitmapFrameEncode> iWICBitmapFrameEncode;
	CComPtr<IWICMetadataQueryWriter> iWICMetadataQueryWriter;
	CComPtr<IWICFormatConverter> iWICFormatConverter;
	CComPtr<IWICBitmap> iWICBitmap;

	//Cursor
	UINT vCursorWidth = 32;
	UINT vCursorHeight = 32;

	//Bytes
	std::vector<BYTE> vScreenBytesCache;

	//Media foundation
	CComPtr<IMFSinkWriterEx> imfSinkWriter;
	CComPtr<IMFDXGIDeviceManager> imfDXGIDeviceManager;
	MediaSettings vMediaSettings{};
	BOOL vMediaCapturing;
	BOOL vMediaWriteLoopAllowed;
	BOOL vMediaWriteLoopFinishedScreen;
	BOOL vMediaWriteLoopFinishedAudio;
	DWORD vOutVideoStreamIndex = 0;
	DWORD vOutAudioStreamIndex = 0;
	ULONGLONG vMediaTimeStartLoop = 0;
	UINT vReferenceTimeFrameDuration = 100000;
	UINT vReferenceTimeToSeconds = 10000000;
	UINT vReferenceTimeToMilliseconds = 10000;
	UINT vReferenceTimeToMicroseconds = 1000;

	//Device Audio
	CComPtr<IMMDevice> iDevice;
	CComPtr<IAudioClient3> iAudioDeviceCapture;
	CComPtr<IAudioClient3> iAudioDeviceRender;
	CComPtr<IAudioCaptureClient> iAudioClientCapture;
	CComPtr<IAudioRenderClient> iAudioClientRender;
	CComHeapPtr<WAVEFORMATEXTENSIBLE> iAudioWaveFormatExCapture;
	CComHeapPtr<WAVEFORMATEXTENSIBLE> iAudioWaveFormatExRender;

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