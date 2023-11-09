#pragma once
#include "CaptureVariables.h"
#include "CaptureReset.cpp"
#include "CaptureDetails.cpp"
#include "CaptureLoop.cpp"
#include "PlayAudio.cpp"
#include "CaptureMessage.cpp"

namespace
{
	BOOL InitializeDirectX(UINT monitorId)
	{
		try
		{
			//Check instance status
			if (vDirectXInstance.vInstanceInitialized)
			{
				std::cout << "DirectX is already initialized for monitor: " << monitorId << std::endl;
				return true;
			}

			//Create D3D11 Device
			D3D_FEATURE_LEVEL iD3DFeatureLevel;
			UINT iD3DCreateFlags = D3D11_CREATE_DEVICE_VIDEO_SUPPORT;
			hResult = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, iD3DCreateFlags, D3DFeatureLevelsArray, D3DFeatureLevelsCount, D3D11_SDK_VERSION, &vDirectXInstance.iD3D11Device0, &iD3DFeatureLevel, &vDirectXInstance.iD3D11DeviceContext0);
			if (FAILED(hResult))
			{
				return false;
			}

			//Convert variables
			hResult = vDirectXInstance.iD3D11Device0->QueryInterface(&vDirectXInstance.iD3D11Device5);
			if (FAILED(hResult))
			{
				return false;
			}

			//Convert variables
			hResult = vDirectXInstance.iD3D11Device5->QueryInterface(&vDirectXInstance.iDxgiDevice4);
			if (FAILED(hResult))
			{
				return false;
			}

			//Convert variables
			hResult = vDirectXInstance.iD3D11Device5->QueryInterface(&vDirectXInstance.iD3D11Multithread);
			if (FAILED(hResult))
			{
				return false;
			}

			//Convert variables
			hResult = vDirectXInstance.iD3D11DeviceContext0->QueryInterface(&vDirectXInstance.iD3D11DeviceContext4);
			if (FAILED(hResult))
			{
				return false;
			}

			//Set multithread protected
			hResult = vDirectXInstance.iD3D11Multithread->SetMultithreadProtected(true);
			if (FAILED(hResult))
			{
				return false;
			}

			//Get DXGI Adapter
			hResult = vDirectXInstance.iDxgiDevice4->GetParent(IID_PPV_ARGS(&vDirectXInstance.iDxgiAdapter4));
			if (FAILED(hResult))
			{
				return false;
			}

			//Get DXGI Output
			hResult = vDirectXInstance.iDxgiAdapter4->EnumOutputs(monitorId, &vDirectXInstance.iDxgiOutput0);
			if (FAILED(hResult))
			{
				return false;
			}

			//Convert variables
			hResult = vDirectXInstance.iDxgiOutput0->QueryInterface(&vDirectXInstance.iDxgiOutput6);
			if (FAILED(hResult))
			{
				return false;
			}

			//Get output description
			hResult = vDirectXInstance.iDxgiOutput6->GetDesc1(&vDirectXInstance.iDxgiOutputDescription);
			if (FAILED(hResult))
			{
				return false;
			}

			//Release resources
			vDirectXInstance.iD3D11Device0.Release();
			vDirectXInstance.iD3D11DeviceContext0.Release();
			vDirectXInstance.iDxgiAdapter4.Release();
			vDirectXInstance.iDxgiOutput0.Release();
			vDirectXInstance.iDxgiOutput6.Release();

			//Update instance status
			vDirectXInstance.vInstanceInitialized = true;
			std::cout << "DirectX initialized for monitor: " << monitorId << std::endl;

			return true;
		}
		catch (...)
		{
			std::cout << "InitializeDirectX for monitor " << monitorId << " failed: " << hResult << std::endl;
			return false;
		}
	}

	BOOL InitializeWgc(UINT monitorId)
	{
		try
		{
			//Convert DXGI to WinRT D3D11 device
			winrt::com_ptr<IInspectable> inspectable;
			hResult = CreateDirect3D11DeviceFromDXGIDevice(vDirectXInstance.iDxgiDevice4, inspectable.put());
			if (FAILED(hResult))
			{
				std::cout << "CreateDirect3D11DeviceFromDXGIDevice failed." << std::endl;
				return false;
			}
			vWgcInstance.vDirect3D11Device = inspectable.as<winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice>();


			//Set capture frame details
			vWgcInstance.vFrameSizeCurrent = { (INT)vCaptureDetails.OriginalWidth, (INT)vCaptureDetails.OriginalHeight };
			if (vCaptureDetails.HDREnabled)
			{
				vWgcInstance.vFramePixelFormat = winrt::Windows::Graphics::DirectX::DirectXPixelFormat::R16G16B16A16Float;
			}
			else
			{
				vWgcInstance.vFramePixelFormat = winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized;
			}

			//Create capture frame pool
			vWgcInstance.vGraphicsCaptureFramePool = winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool::CreateFreeThreaded(vWgcInstance.vDirect3D11Device, vWgcInstance.vFramePixelFormat, 1, vWgcInstance.vFrameSizeCurrent);

			//Set graphic capture item
			auto interop_factory = winrt::get_activation_factory<winrt::Windows::Graphics::Capture::GraphicsCaptureItem, IGraphicsCaptureItemInterop>();
			auto interop_uuid = winrt::guid_of<winrt::Windows::Graphics::Capture::IGraphicsCaptureItem>();

			////Create capture item for window
			//hResult = interop_factory->CreateForWindow(GetForegroundWindow(), interop_uuid, (void**)&vWgcInstance.vGraphicsCaptureItem);
			//if (FAILED(hResult))
			//{
			//	std::cout << "CreateForWindow failed." << std::endl;
			//	return false;
			//}

			//Create capture item for monitor
			hResult = interop_factory->CreateForMonitor(vDirectXInstance.iDxgiOutputDescription.Monitor, interop_uuid, (void**)&vWgcInstance.vGraphicsCaptureItem);
			if (FAILED(hResult))
			{
				std::cout << "CreateForMonitor failed." << std::endl;
				return false;
			}

			//Allow borderless capture
			winrt::Windows::Graphics::Capture::GraphicsCaptureAccess::RequestAccessAsync(winrt::Windows::Graphics::Capture::GraphicsCaptureAccessKind::Borderless);

			//Start capture session
			vWgcInstance.vGraphicsCaptureSession = vWgcInstance.vGraphicsCaptureFramePool.CreateCaptureSession(vWgcInstance.vGraphicsCaptureItem);
			vWgcInstance.vGraphicsCaptureSession.IsCursorCaptureEnabled(true);
			vWgcInstance.vGraphicsCaptureSession.IsBorderRequired(false);
			vWgcInstance.vGraphicsCaptureSession.StartCapture();

			//Release resources
			vDirectXInstance.iDxgiDevice4.Release();
			vWgcInstance.vDirect3D11Device = NULL;

			//Update thread variables
			vWgcInstance.vCaptureStatusLoopAllowed = true;

			//Loop capture check status
			std::thread threadLoopCheckStatus(LoopCaptureStatus);
			threadLoopCheckStatus.detach();

			//Update instance status
			vWgcInstance.vInstanceInitialized = true;
			std::cout << "Windows Graphics Capture initialized for monitor: " << monitorId << std::endl;

			return true;
		}
		catch (...)
		{
			std::cout << "InitializeWgc for monitor " << monitorId << " failed: " << hResult << std::endl;
			return false;
		}
	}

	BOOL InitializeRenderTargetView()
	{
		try
		{
			//Create render target view texture
			D3D11_TEXTURE2D_DESC iD3DTexture2D0DescRenderTargetView{};
			iD3DTexture2D0DescRenderTargetView.Width = vCaptureDetails.OutputWidth;
			iD3DTexture2D0DescRenderTargetView.Height = vCaptureDetails.OutputHeight;
			iD3DTexture2D0DescRenderTargetView.MipLevels = 1;
			iD3DTexture2D0DescRenderTargetView.ArraySize = 1;
			iD3DTexture2D0DescRenderTargetView.Format = vCaptureInstance.vCaptureDxgiFormat;
			iD3DTexture2D0DescRenderTargetView.SampleDesc.Count = 1;
			iD3DTexture2D0DescRenderTargetView.SampleDesc.Quality = 0;
			iD3DTexture2D0DescRenderTargetView.Usage = D3D11_USAGE_DEFAULT;
			iD3DTexture2D0DescRenderTargetView.BindFlags = D3D11_BIND_RENDER_TARGET;
			iD3DTexture2D0DescRenderTargetView.CPUAccessFlags = 0;
			iD3DTexture2D0DescRenderTargetView.MiscFlags = 0;
			hResult = vDirectXInstance.iD3D11Device5->CreateTexture2D(&iD3DTexture2D0DescRenderTargetView, NULL, &vCaptureInstance.iD3D11Texture2D0RenderTargetView);
			if (FAILED(hResult))
			{
				return false;
			}

			//Create and set render target view
			hResult = vDirectXInstance.iD3D11Device5->CreateRenderTargetView(vCaptureInstance.iD3D11Texture2D0RenderTargetView, NULL, &vDirectXInstance.iD3D11RenderTargetView0);
			if (FAILED(hResult))
			{
				return false;
			}
			vDirectXInstance.iD3D11DeviceContext4->OMSetRenderTargets(1, &vDirectXInstance.iD3D11RenderTargetView0, NULL);

			//Clear render target view
			vDirectXInstance.iD3D11DeviceContext4->ClearRenderTargetView(vDirectXInstance.iD3D11RenderTargetView0, ColorRgbaBlack);

			//Release resources
			vDirectXInstance.iD3D11RenderTargetView0.Release();

			return true;
		}
		catch (...)
		{
			std::cout << "InitializeRenderTargetView failed: " << hResult << std::endl;
			return false;
		}
	}

	BOOL InitializeViewPort()
	{
		try
		{
			//Create and set viewport
			D3D11_VIEWPORT iD3D11ViewPort{};
			iD3D11ViewPort.Width = vCaptureDetails.OutputWidth;
			iD3D11ViewPort.Height = vCaptureDetails.OutputHeight;
			vDirectXInstance.iD3D11DeviceContext4->RSSetViewports(1, &iD3D11ViewPort);

			return true;
		}
		catch (...)
		{
			std::cout << "InitializeViewPort failed: " << hResult << std::endl;
			return false;
		}
	}

	BOOL InitializeShaders()
	{
		try
		{
			//Load shaders from file
			hResult = D3DCompileFromFile(L"Resources\\VertexShader.hlsl", 0, 0, "main", "vs_5_0", 0, 0, &vDirectXInstance.iD3DBlob0VertexShader, 0);
			if (FAILED(hResult))
			{
				return false;
			}
			hResult = D3DCompileFromFile(L"Resources\\PixelShader.hlsl", 0, 0, "main", "ps_5_0", 0, 0, &vDirectXInstance.iD3DBlob0PixelShader, 0);
			if (FAILED(hResult))
			{
				return false;
			}

			//Create shaders from blob
			hResult = vDirectXInstance.iD3D11Device5->CreateVertexShader(vDirectXInstance.iD3DBlob0VertexShader->GetBufferPointer(), vDirectXInstance.iD3DBlob0VertexShader->GetBufferSize(), NULL, &vDirectXInstance.iD3D11VertexShader0);
			if (FAILED(hResult))
			{
				return false;
			}
			hResult = vDirectXInstance.iD3D11Device5->CreatePixelShader(vDirectXInstance.iD3DBlob0PixelShader->GetBufferPointer(), vDirectXInstance.iD3DBlob0PixelShader->GetBufferSize(), NULL, &vDirectXInstance.iD3D11PixelShader0);
			if (FAILED(hResult))
			{
				return false;
			}

			//Create and set input layout
			hResult = vDirectXInstance.iD3D11Device5->CreateInputLayout(InputElementsArray, InputElementsCount, vDirectXInstance.iD3DBlob0VertexShader->GetBufferPointer(), vDirectXInstance.iD3DBlob0VertexShader->GetBufferSize(), &vDirectXInstance.iD3D11InputLayout0);
			if (FAILED(hResult))
			{
				return false;
			}
			vDirectXInstance.iD3D11DeviceContext4->IASetInputLayout(vDirectXInstance.iD3D11InputLayout0);

			//Set shaders
			vDirectXInstance.iD3D11DeviceContext4->VSSetShader(vDirectXInstance.iD3D11VertexShader0, NULL, 0);
			vDirectXInstance.iD3D11DeviceContext4->PSSetShader(vDirectXInstance.iD3D11PixelShader0, NULL, 0);

			//Release resources
			vDirectXInstance.iD3D11Buffer0.Release();
			vDirectXInstance.iD3DBlob0VertexShader.Release();
			vDirectXInstance.iD3DBlob0PixelShader.Release();
			vDirectXInstance.iD3D11VertexShader0.Release();
			vDirectXInstance.iD3D11PixelShader0.Release();
			vDirectXInstance.iD3D11InputLayout0.Release();

			return true;
		}
		catch (...)
		{
			std::cout << "InitializeShaders failed: " << hResult << std::endl;
			return false;
		}
	}

	BOOL SetShaderVariables()
	{
		try
		{
			//Create shader variables
			ShaderVariables shaderVariables{};
			shaderVariables.HDRtoSDR = vCaptureDetails.HDRtoSDR;
			shaderVariables.HDRPaperWhite = vCaptureSettings.HDRPaperWhite;
			shaderVariables.HDRMaximumNits = vCaptureSettings.HDRMaximumNits;
			shaderVariables.SDRWhiteLevel = vCaptureDetails.SDRWhiteLevel;
			shaderVariables.Saturation = vCaptureSettings.Saturation;
			shaderVariables.RedChannel = vCaptureSettings.RedChannel;
			shaderVariables.GreenChannel = vCaptureSettings.GreenChannel;
			shaderVariables.BlueChannel = vCaptureSettings.BlueChannel;
			shaderVariables.Brightness = vCaptureSettings.Brightness;
			shaderVariables.Contrast = vCaptureSettings.Contrast;
			shaderVariables.Gamma = vCaptureSettings.Gamma;
			shaderVariables.Blur = vCaptureSettings.Blur;

			//Create buffer description
			D3D11_BUFFER_DESC bufferDescription{};
			bufferDescription.ByteWidth = sizeof(ShaderVariables);
			bufferDescription.Usage = D3D11_USAGE_DEFAULT;
			bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bufferDescription.CPUAccessFlags = 0;
			bufferDescription.MiscFlags = 0;
			bufferDescription.StructureByteStride = 0;

			//Create subresource data
			D3D11_SUBRESOURCE_DATA subResourceData{};
			subResourceData.pSysMem = &shaderVariables;

			//Create shader variables buffer
			hResult = vDirectXInstance.iD3D11Device5->CreateBuffer(&bufferDescription, &subResourceData, &vDirectXInstance.iD3D11Buffer0);
			if (FAILED(hResult))
			{
				return false;
			}

			//Set shader variables
			vDirectXInstance.iD3D11DeviceContext4->PSSetConstantBuffers(0, 1, &vDirectXInstance.iD3D11Buffer0);

			//Release resources
			vDirectXInstance.iD3D11Buffer0.Release();

			return true;
		}
		catch (...)
		{
			std::cout << "SetShaderVariables failed: " << hResult << std::endl;
			return false;
		}
	}

	BOOL SetCaptureDetails(UINT monitorId)
	{
		try
		{
			//Get and set HDR details
			vCaptureDetails.HDREnabled = vDirectXInstance.iDxgiOutputDescription.ColorSpace == DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020 || vDirectXInstance.iDxgiOutputDescription.ColorSpace == DXGI_COLOR_SPACE_RGB_STUDIO_G2084_NONE_P2020;
			if (vCaptureDetails.HDREnabled)
			{
				vCaptureDetails.HDRtoSDR = vCaptureSettings.HDRtoSDR;
				if (vCaptureDetails.HDRtoSDR)
				{
					vCaptureInstance.iWicPixelFormatGuidSource = GUID_WICPixelFormat32bppBGRA;
					vCaptureInstance.vCaptureDxgiFormat = DXGI_FORMAT_B8G8R8A8_UNORM; //DXGI_FORMAT_B8G8R8A8_UNORM_SRGB
					vCaptureDetails.PixelByteSize = 4;
					vCaptureDetails.SDRWhiteLevel = GetMonitorSDRWhiteLevel(monitorId);
				}
				else
				{
					vCaptureInstance.iWicPixelFormatGuidSource = GUID_WICPixelFormat64bppRGBAHalf;
					vCaptureInstance.vCaptureDxgiFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
					vCaptureDetails.PixelByteSize = 8;
				}
			}
			else
			{
				vCaptureDetails.HDRtoSDR = false;
				vCaptureInstance.iWicPixelFormatGuidSource = GUID_WICPixelFormat32bppBGRA;
				vCaptureInstance.vCaptureDxgiFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
				vCaptureDetails.PixelByteSize = 4;
			}

			//Get monitor details
			UINT monitorPixelsWidth = 0;
			UINT monitorPixelsHeight = 0;
			UINT monitorRefreshRate = GetMonitorRefreshRate(vDirectXInstance.iDxgiOutputDescription.Monitor);
			GetMonitorResolution(vDirectXInstance.iDxgiOutputDescription.Monitor, monitorPixelsWidth, monitorPixelsHeight);

			//Update capture variables
			vCaptureDetails.OriginalWidth = monitorPixelsWidth;
			vCaptureDetails.OriginalHeight = monitorPixelsHeight;
			vCaptureDetails.RefreshRate = monitorRefreshRate;
			vCaptureInstance.vCaptureTextureResizing = vCaptureSettings.MaxPixelDimension != 0 && vCaptureDetails.OriginalWidth > vCaptureSettings.MaxPixelDimension && vCaptureDetails.OriginalHeight > vCaptureSettings.MaxPixelDimension;
			if (vCaptureInstance.vCaptureTextureResizing)
			{
				DOUBLE resizedWidth = 0.01;
				DOUBLE resizedHeight = 0.01;
				UINT minDimension = min(vCaptureDetails.OriginalWidth, vCaptureDetails.OriginalHeight);
				//Find nearest full pixel dimensions to keep original ratio
				while (resizedWidth != (UINT)resizedWidth || resizedHeight != (UINT)resizedHeight)
				{
					DOUBLE differenceRatio = (DOUBLE)minDimension / vCaptureSettings.MaxPixelDimension;
					resizedWidth = vCaptureDetails.OriginalWidth / differenceRatio;
					resizedHeight = vCaptureDetails.OriginalHeight / differenceRatio;
					vCaptureSettings.MaxPixelDimension++;
				}
				vCaptureDetails.OutputWidth = resizedWidth;
				vCaptureDetails.OutputHeight = resizedHeight;
			}
			else
			{
				vCaptureDetails.OutputWidth = vCaptureDetails.OriginalWidth;
				vCaptureDetails.OutputHeight = vCaptureDetails.OriginalHeight;
			}
			std::cout << "Screen capture output, Width: " << vCaptureDetails.OutputWidth << " Height: " << vCaptureDetails.OutputHeight << std::endl;
			vCaptureDetails.WidthByteSize = vCaptureDetails.OutputWidth * vCaptureDetails.PixelByteSize;
			vCaptureDetails.TotalByteSize = vCaptureDetails.OutputWidth * vCaptureDetails.OutputHeight * vCaptureDetails.PixelByteSize;
			vCaptureInstance.vCaptureTextureMipLevels = 1 + log2(max(vCaptureDetails.OutputWidth, vCaptureDetails.OutputHeight));

			return true;
		}
		catch (...)
		{
			std::cout << "SetCaptureDetails failed: " << hResult << std::endl;
			return false;
		}
	}

	BOOL CaptureInitializeCode(CaptureSettings captureSettings, CaptureDetails& captureDetails, BOOL forceInitialize)
	{
		try
		{
			//Check capture initialized
			if (!forceInitialize && vCaptureInstance.vInstanceInitialized)
			{
				//Return capture details
				captureDetails = vCaptureDetails;

				//Return result
				std::cout << "Capture is already initialized." << std::endl;
				return true;
			}

			std::cout << "Initializing capture..." << std::endl;

			//Disable assert reporting
			_CrtSetReportMode(_CRT_ASSERT, 0);

			//Set process dpi awareness
			SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);

			//Reset all used variables
			WgcResetVariablesAll();
			DirectXResetVariablesAll();
			CaptureResetVariablesAll();

			//Update capture settings
			vCaptureSettings = captureSettings;

			//Create message capture window
			WindowCreateHiddenMessageCapture();

			//Initialize DirectX
			if (!InitializeDirectX(captureSettings.MonitorId))
			{
				//Reset all used variables
				WgcResetVariablesAll();
				DirectXResetVariablesAll();
				CaptureResetVariablesAll();

				//Play audio effect
				PlayAudio(L"Assets\\Capture\\CaptureFailed.mp3");

				return false;
			}

			//Set capture details
			if (!SetCaptureDetails(captureSettings.MonitorId))
			{
				//Reset all used variables
				WgcResetVariablesAll();
				DirectXResetVariablesAll();
				CaptureResetVariablesAll();

				//Play audio effect
				PlayAudio(L"Assets\\Capture\\CaptureFailed.mp3");

				return false;
			}

			//Initialize Windows Graphics Capture
			if (!InitializeWgc(captureSettings.MonitorId))
			{
				//Reset all used variables
				WgcResetVariablesAll();
				DirectXResetVariablesAll();
				CaptureResetVariablesAll();

				//Play audio effect
				PlayAudio(L"Assets\\Capture\\CaptureFailed.mp3");

				return false;
			}

			//Initialize shaders
			if (!InitializeShaders())
			{
				//Reset all used variables
				WgcResetVariablesAll();
				DirectXResetVariablesAll();
				CaptureResetVariablesAll();

				//Play audio effect
				PlayAudio(L"Assets\\Capture\\CaptureFailed.mp3");

				return false;
			}

			//Initialize render target view
			if (!InitializeRenderTargetView())
			{
				//Reset all used variables
				WgcResetVariablesAll();
				DirectXResetVariablesAll();
				CaptureResetVariablesAll();

				//Play audio effect
				PlayAudio(L"Assets\\Capture\\CaptureFailed.mp3");

				return false;
			}

			//Initialize view port
			if (!InitializeViewPort())
			{
				//Reset all used variables
				DirectXResetVariablesAll();
				CaptureResetVariablesAll();

				//Play audio effect
				PlayAudio(L"Assets\\Capture\\CaptureFailed.mp3");

				return false;
			}

			//Set shader variables
			if (!SetShaderVariables())
			{
				//Reset all used variables
				WgcResetVariablesAll();
				DirectXResetVariablesAll();
				CaptureResetVariablesAll();

				//Play audio effect
				PlayAudio(L"Assets\\Capture\\CaptureFailed.mp3");

				return false;
			}

			//Update view vertex vertices
			if (!RenderUpdateVertex(VertexVerticesArrayScreen, VertexVerticesCountScreen))
			{
				//Reset all used variables
				WgcResetVariablesAll();
				DirectXResetVariablesAll();
				CaptureResetVariablesAll();

				//Play audio effect
				PlayAudio(L"Assets\\Capture\\CaptureFailed.mp3");

				return false;
			}

			//Update variables
			vCaptureInstance.vInstanceInitialized = true;

			//Return capture details
			captureDetails = vCaptureDetails;

			//Return result
			std::cout << "Capture initialized successfully." << std::endl;
			return true;
		}
		catch (...)
		{
			std::cout << "InitializeCapture failed: " << hResult << std::endl;

			//Reset all used variables
			WgcResetVariablesAll();
			DirectXResetVariablesAll();
			CaptureResetVariablesAll();

			//Play audio effect
			PlayAudio(L"Assets\\Capture\\CaptureFailed.mp3");

			return false;
		}
	}
}