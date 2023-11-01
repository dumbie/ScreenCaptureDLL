#pragma once
#include "CaptureIncludes.h"

namespace
{
	//Enumerators
	enum ImageFormats
	{
		JXR = 0,
		JPG = 1,
		PNG = 2,
		BMP = 3,
		TIF = 4,
		HEIF = 5
	};

	enum VideoFormats
	{
		H264 = 0,
		HEVC = 1
	};

	enum AudioFormats
	{
		MP3 = 0,
		AAC = 1,
		FLAC = 2
	};

	//Structures
	struct MediaSettings
	{
		AudioFormats AudioFormat = AAC;
		UINT AudioChannels = 2;
		UINT AudioBitRate = 256;
		UINT AudioBitDepth = 16;
		UINT AudioSampleRate = 48000;
		VideoFormats VideoFormat = HEVC;
		UINT VideoQuality = 60;
		UINT VideoFrameRate = 60;
	};

	struct CaptureSettings
	{
		UINT MonitorId = 0;
		UINT MaxPixelDimension = 0;
		BOOL SoundEffect = true;
		BOOL MouseIgnoreMovement = false;
		BOOL MouseDrawCursor = true;
		BOOL HDRtoSDR = false;
		FLOAT HDRPaperWhite = 80.0F;
		FLOAT HDRMaximumNits = 1000.0F;
		FLOAT Saturation = 1.0F;
		FLOAT RedChannel = 1.0F;
		FLOAT GreenChannel = 1.0F;
		FLOAT BlueChannel = 1.0F;
		FLOAT Brightness = 1.0F;
		FLOAT Contrast = 0.0F;
		FLOAT Gamma = 1.0F;
		FLOAT Blur = 0.0F;
	};

	struct CaptureDetails
	{
		UINT OriginalWidth;
		UINT OriginalHeight;
		UINT OutputWidth;
		UINT OutputHeight;
		UINT RefreshRate;
		BOOL HDREnabled;
		BOOL HDRtoSDR;
		FLOAT SDRWhiteLevel;
		UINT PixelByteSize;
		UINT WidthByteSize;
		UINT TotalByteSize;
	};

	struct ShaderVariables
	{
		BOOL HDRtoSDR;
		FLOAT HDRPaperWhite;
		FLOAT HDRMaximumNits;
		FLOAT SDRWhiteLevel;
		FLOAT Saturation;
		FLOAT RedChannel;
		FLOAT GreenChannel;
		FLOAT BlueChannel;
		FLOAT Brightness;
		FLOAT Contrast;
		FLOAT Gamma;
		FLOAT Blur;
	};

	struct DirectXInstance
	{
		//Status
		BOOL vInstanceInitialized;

		//Information
		DXGI_OUTPUT_DESC1 iDxgiOutputDescription;
		DXGI_OUTDUPL_DESC iDxgiOutputDuplicationDescription;

		//Direct3D
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
		CComPtr<ID3D11BlendState> iD3D11BlendState0;

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
	};

	struct CaptureInstance
	{
		//Status
		BOOL vInstanceInitialized;
		BOOL vCaptureTextureResizing;
		UINT vCaptureTextureMipLevels;
		UINT vCaptureFailCount = 0;

		//Information
		CaptureSettings vCaptureSettings;
		CaptureDetails vCaptureDetails;
		DXGI_FORMAT vCaptureDxgiFormat;

		//Cursor
		UINT vCursorWidth = 32;
		UINT vCursorHeight = 32;

		//Bytes
		std::vector<BYTE> vScreenBytesCache;

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

		//Device Audio
		CComPtr<IMMDevice> iDevice;
		CComPtr<IAudioClient3> iAudioDeviceCapture;
		CComPtr<IAudioClient3> iAudioDeviceRender;
		CComPtr<IAudioCaptureClient> iAudioClientCapture;
		CComPtr<IAudioRenderClient> iAudioClientRender;
		CComHeapPtr<WAVEFORMATEXTENSIBLE> iAudioWaveFormatExCapture;
		CComHeapPtr<WAVEFORMATEXTENSIBLE> iAudioWaveFormatExRender;

		//Textures
		CComPtr<IDXGIResource> iDxgiResource0;
		CComPtr<ID3D11Texture2D> iD3D11Texture2D0CpuRead;
		CComPtr<ID3D11Texture2D> iD3D11Texture2D0Screen;
		CComPtr<ID3D11Texture2D> iD3D11Texture2D0Cursor;
		CComPtr<ID3D11Texture2D> iD3D11Texture2D0RenderTargetView;
	};

	struct VertexVertice
	{
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT2 TexCoord;
	};
};