#pragma once
#include "CaptureVariables.h"
#include "CaptureReset.cpp"
#include "CaptureDetails.cpp"
#include "CaptureLoop.cpp"
#include "PlayAudio.cpp"

namespace
{
	CaptureResult InitializeDirectXCreateDevice()
	{
		try
		{
			for (int arrayCount = 0; arrayCount < D3DFeatureLevelsCount; arrayCount++)
			{
				D3D_FEATURE_LEVEL iD3DFeatureLevel;
				D3D_FEATURE_LEVEL iD3DFeatureLevels[] = { D3DFeatureLevelsArray[arrayCount] };
				UINT iD3DCreateFlags = D3D11_CREATE_DEVICE_VIDEO_SUPPORT | D3D11_CREATE_DEVICE_BGRA_SUPPORT;
				hResult = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, iD3DCreateFlags, iD3DFeatureLevels, 1, D3D11_SDK_VERSION, (ID3D11Device**)&vDirectXInstance.iD3D11Device5, &iD3DFeatureLevel, (ID3D11DeviceContext**)&vDirectXInstance.iD3D11DeviceContext4);
				if (SUCCEEDED(hResult))
				{
					AVDebugWriteLine("Created DirectX device with feature level: " << D3DFeatureLevelsArray[arrayCount]);
					return { .Status = CaptureStatus::Success };
				}
			}
		}
		catch (...) {}
		return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"InitializeDirectXCreateDevice failed") };
	}

	CaptureResult InitializeDirectXCreate(UINT monitorId)
	{
		try
		{
			//Create DirectX device
			capResult = InitializeDirectXCreateDevice();
			if (capResult.Status != CaptureStatus::Success)
			{
				return capResult;
			}

			//Convert variables
			hResult = vDirectXInstance.iD3D11Device5->QueryInterface(&vDirectXInstance.iDxgiDevice4);
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"Failed converting iDxgiDevice4") };
			}

			//Convert variables
			hResult = vDirectXInstance.iD3D11Device5->QueryInterface(&vDirectXInstance.iD3D11Multithread0);
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"Failed converting iD3D11Multithread0") };
			}

			//Get DXGI Adapter
			hResult = vDirectXInstance.iDxgiDevice4->GetParent(IID_PPV_ARGS(&vDirectXInstance.iDxgiAdapter4));
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"Failed getting DXGI adapter") };
			}

			//Get DXGI Output
			hResult = vDirectXInstance.iDxgiAdapter4->EnumOutputs(monitorId, (IDXGIOutput**)&vDirectXInstance.iDxgiOutput6);
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"Failed getting DXGI output") };
			}

			//Get DXGI Factory
			hResult = vDirectXInstance.iDxgiAdapter4->GetParent(IID_PPV_ARGS(&vDirectXInstance.iDxgiFactory7));
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"Failed getting DXGI factory") };
			}

			//Get output description
			hResult = vDirectXInstance.iDxgiOutput6->GetDesc1(&vDirectXInstance.iDxgiOutputDescription1);
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"Failed getting output description") };
			}

			//Return result
			AVDebugWriteLine("DirectX initialized successfully for monitor: " << monitorId);
			return { .Status = CaptureStatus::Success };
		}
		catch (...)
		{
			//Return result
			return { .Status = CaptureStatus::Failed, .Message = SysAllocString(L"InitializeDirectXCreate failed") };
		}
	}

	CaptureResult InitializeDirectXSetKMT()
	{
		try
		{
			HANDLE currentProcess = GetCurrentProcess();
			for (int arrayCount = 0; arrayCount < D3DKMTSchedulingPriorityClassCount; arrayCount++)
			{
				ntStatus = D3DKMTSetProcessSchedulingPriorityClass(currentProcess, D3DKMTSchedulingPriorityClassArray[arrayCount]);
				if (NT_SUCCESS(ntStatus))
				{
					AVDebugWriteLine("Set D3DKMT ProcessSchedulingPriorityClass: " << D3DKMTSchedulingPriorityClassArray[arrayCount]);
					return { .Status = CaptureStatus::Success };
				}
			}
		}
		catch (...) {}
		return { .Status = CaptureStatus::Failed, .ResultCode = ntStatus, .Message = SysAllocString(L"InitializeDirectXSetKMT failed") };
	}

	CaptureResult InitializeDirectXTweaks()
	{
		try
		{
			//Set process scheduler priority
			capResult = InitializeDirectXSetKMT();
			if (capResult.Status != CaptureStatus::Success)
			{
				return capResult;
			}

			//Set maximum queue back buffer frames
			hResult = vDirectXInstance.iDxgiDevice4->SetMaximumFrameLatency(16);
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"SetMaximumFrameLatency failed") };
			}

			//Set gpu thread scheduler priority
			hResult = vDirectXInstance.iDxgiDevice4->SetGPUThreadPriority(0);
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"SetGPUThreadPriority failed") };
			}

			//Set multithread protection
			hResult = vDirectXInstance.iD3D11Multithread0->SetMultithreadProtected(true);
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"SetMultithreadProtected failed") };
			}

			//Return result
			AVDebugWriteLine("DirectX performance tweaks applied.");
			return { .Status = CaptureStatus::Success };
		}
		catch (...)
		{
			//Return result
			return { .Status = CaptureStatus::Failed, .Message = SysAllocString(L"InitializeDirectXTweaks failed") };
		}
	}

	CaptureResult InitializeDirectXLoop(UINT monitorId)
	{
		try
		{
			for (int retryCount = 0; retryCount < 6; retryCount++)
			{
				capResult = InitializeDirectXCreate(monitorId);
				if (capResult.Status == CaptureStatus::Success)
				{
					return capResult;
				}
				else
				{
					AVDebugWriteLine("Failed to initialize DirectX retrying: " << retryCount << "x");
					AVHighResDelay(1000);
				}
			}
		}
		catch (...) {}
		return { .Status = CaptureStatus::Failed, .Message = SysAllocString(L"InitializeDirectXLoop failed") };
	}

	CaptureResult InitializeSamplerState()
	{
		try
		{
			//Create sampler description
			D3D11_SAMPLER_DESC iD3DSamplerDescription{};
			iD3DSamplerDescription.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
			iD3DSamplerDescription.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
			iD3DSamplerDescription.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
			iD3DSamplerDescription.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
			iD3DSamplerDescription.MipLODBias = 0.0F;
			iD3DSamplerDescription.MaxAnisotropy = 0;
			iD3DSamplerDescription.ComparisonFunc = D3D11_COMPARISON_NEVER;
			iD3DSamplerDescription.BorderColor[0] = 1.0F;
			iD3DSamplerDescription.BorderColor[1] = 1.0F;
			iD3DSamplerDescription.BorderColor[2] = 1.0F;
			iD3DSamplerDescription.BorderColor[3] = 1.0F;
			iD3DSamplerDescription.MinLOD = -D3D11_FLOAT32_MAX;
			iD3DSamplerDescription.MaxLOD = D3D11_FLOAT32_MAX;

			//Create sampler state
			hResult = vDirectXInstance.iD3D11Device5->CreateSamplerState(&iD3DSamplerDescription, &vDirectXInstance.iD3D11SamplerState0);
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"CreateSamplerState failed") };
			}

			//Set sampler state
			vDirectXInstance.iD3D11DeviceContext4->PSSetSamplers(0, 1, &vDirectXInstance.iD3D11SamplerState0);

			//Return result
			return { .Status = CaptureStatus::Success };
		}
		catch (...)
		{
			//Return result
			return { .Status = CaptureStatus::Failed, .Message = SysAllocString(L"InitializeSamplerState failed") };
		}
	}

	CaptureResult InitializeWgc()
	{
		try
		{
			//Convert DXGI to WinRT D3D11 device
			winrt::com_ptr<IInspectable> inspectable;
			hResult = CreateDirect3D11DeviceFromDXGIDevice(vDirectXInstance.iDxgiDevice4, inspectable.put());
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"CreateDirect3D11DeviceFromDXGIDevice failed") };
			}
			vWgcInstance.vGraphicsD3D11Device = inspectable.as<winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice>();

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
			vWgcInstance.vGraphicsCaptureFramePool = winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool::CreateFreeThreaded(vWgcInstance.vGraphicsD3D11Device, vWgcInstance.vFramePixelFormat, 1, vWgcInstance.vFrameSizeCurrent);

			//Set graphic capture item
			auto interop_factory = winrt::get_activation_factory<winrt::Windows::Graphics::Capture::GraphicsCaptureItem, IGraphicsCaptureItemInterop>();
			auto interop_uuid = winrt::guid_of<winrt::Windows::Graphics::Capture::IGraphicsCaptureItem>();

			////Create capture item for window
			//hResult = interop_factory->CreateForWindow(GetForegroundWindow(), interop_uuid, (void**)&vWgcInstance.vGraphicsCaptureItem);
			//if (FAILED(hResult))
			//{
			//	return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"CreateForWindow failed") };
			//}

			//Create capture item for monitor
			hResult = interop_factory->CreateForMonitor(vDirectXInstance.iDxgiOutputDescription1.Monitor, interop_uuid, (void**)&vWgcInstance.vGraphicsCaptureItem);
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"CreateForMonitor failed") };
			}

			//Create capture session
			vWgcInstance.vGraphicsCaptureSession = vWgcInstance.vGraphicsCaptureFramePool.CreateCaptureSession(vWgcInstance.vGraphicsCaptureItem);

			//Hide or show mouse cursor
			vWgcInstance.vGraphicsCaptureSession.IsCursorCaptureEnabled(vCaptureSettings.DrawMouseCursor);

			try
			{
				//Hide or show capture border
				winrt::Windows::Graphics::Capture::GraphicsCaptureAccess::RequestAccessAsync(winrt::Windows::Graphics::Capture::GraphicsCaptureAccessKind::Borderless);
				vWgcInstance.vGraphicsCaptureSession.IsBorderRequired(vCaptureSettings.DrawBorder);
			}
			catch (...)
			{
				AVDebugWriteLine("Failed to show or hide capture border, not supported?");
			}

			//Start capture session
			vWgcInstance.vGraphicsCaptureSession.StartCapture();

			//Return result
			AVDebugWriteLine("Windows Graphics Capture initialized.");
			return { .Status = CaptureStatus::Success };
		}
		catch (...)
		{
			//Return result
			return { .Status = CaptureStatus::Failed, .Message = SysAllocString(L"InitializeWgc failed") };
		}
	}

	CaptureResult InitializeRenderTargetView()
	{
		try
		{
			//Create render target view texture
			D3D11_TEXTURE2D_DESC iD3DTexture2D0DescRenderTargetView{};
			iD3DTexture2D0DescRenderTargetView.Width = vCaptureDetails.OutputWidth;
			iD3DTexture2D0DescRenderTargetView.Height = vCaptureDetails.OutputHeight;
			iD3DTexture2D0DescRenderTargetView.MipLevels = vCaptureInstance.vCaptureTextureMipLevels;
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
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"CreateTexture2D RenderTargetView failed") };
			}

			//Create and set render target view
			hResult = vDirectXInstance.iD3D11Device5->CreateRenderTargetView(vCaptureInstance.iD3D11Texture2D0RenderTargetView, NULL, &vDirectXInstance.iD3D11RenderTargetView0);
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"CreateRenderTargetView failed") };
			}
			vDirectXInstance.iD3D11DeviceContext4->OMSetRenderTargets(1, &vDirectXInstance.iD3D11RenderTargetView0, NULL);

			//Clear render target view
			vDirectXInstance.iD3D11DeviceContext4->ClearRenderTargetView(vDirectXInstance.iD3D11RenderTargetView0, ColorRgbaBlack);

			//Return result
			return { .Status = CaptureStatus::Success };
		}
		catch (...)
		{
			//Return result
			return { .Status = CaptureStatus::Failed, .Message = SysAllocString(L"InitializeRenderTargetView failed") };
		}
	}

	CaptureResult InitializeViewPort()
	{
		try
		{
			//Create and set viewport
			D3D11_VIEWPORT iD3D11ViewPort{};
			iD3D11ViewPort.Width = vCaptureDetails.OutputWidth;
			iD3D11ViewPort.Height = vCaptureDetails.OutputHeight;
			vDirectXInstance.iD3D11DeviceContext4->RSSetViewports(1, &iD3D11ViewPort);

			//Return result
			return { .Status = CaptureStatus::Success };
		}
		catch (...)
		{
			//Return result
			return { .Status = CaptureStatus::Failed, .Message = SysAllocString(L"InitializeViewPort failed") };
		}
	}

	CaptureResult InitializeShaders()
	{
		try
		{
			//Load shaders from file
			hResult = D3DCompileFromFile(L"Shaders\\VertexShader.hlsl", 0, 0, "main", "vs_5_0", 0, 0, &vDirectXInstance.iD3DBlobShaderVertex0, 0);
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"Failed loading vertex shader") };
			}
			hResult = D3DCompileFromFile(L"Shaders\\PixelShader.hlsl", 0, 0, "main", "ps_5_0", 0, 0, &vDirectXInstance.iD3DBlobShaderPixel0, 0);
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"Failed loading pixel shader") };
			}

			//Create shaders from blob
			hResult = vDirectXInstance.iD3D11Device5->CreateVertexShader(vDirectXInstance.iD3DBlobShaderVertex0->GetBufferPointer(), vDirectXInstance.iD3DBlobShaderVertex0->GetBufferSize(), NULL, &vDirectXInstance.iD3D11ShaderVertex0);
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"Failed creating vertex shader") };
			}
			hResult = vDirectXInstance.iD3D11Device5->CreatePixelShader(vDirectXInstance.iD3DBlobShaderPixel0->GetBufferPointer(), vDirectXInstance.iD3DBlobShaderPixel0->GetBufferSize(), NULL, &vDirectXInstance.iD3D11ShaderPixel0);
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"Failed creating pixel shader") };
			}

			//Create and set input layout
			hResult = vDirectXInstance.iD3D11Device5->CreateInputLayout(InputElementsArray, InputElementsCount, vDirectXInstance.iD3DBlobShaderVertex0->GetBufferPointer(), vDirectXInstance.iD3DBlobShaderVertex0->GetBufferSize(), &vDirectXInstance.iD3D11InputLayout0);
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"Failed creating input layout") };
			}
			vDirectXInstance.iD3D11DeviceContext4->IASetInputLayout(vDirectXInstance.iD3D11InputLayout0);

			//Set shaders
			vDirectXInstance.iD3D11DeviceContext4->VSSetShader(vDirectXInstance.iD3D11ShaderVertex0, NULL, 0);
			vDirectXInstance.iD3D11DeviceContext4->PSSetShader(vDirectXInstance.iD3D11ShaderPixel0, NULL, 0);

			//Return result
			return { .Status = CaptureStatus::Success };
		}
		catch (...)
		{
			//Return result
			return { .Status = CaptureStatus::Failed, .Message = SysAllocString(L"InitializeShaders failed") };
		}
	}

	CaptureResult SetShaderVariables()
	{
		try
		{
			//Create shader variables
			ShaderVariables shaderVariables{};
			shaderVariables.TextureFilterUse = vCaptureInstance.vCaptureTextureResizing;
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
			bufferDescription.ByteWidth = sizeof(shaderVariables);
			bufferDescription.Usage = D3D11_USAGE_DEFAULT;
			bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bufferDescription.CPUAccessFlags = 0;
			bufferDescription.MiscFlags = 0;
			bufferDescription.StructureByteStride = 0;

			//Create subresource data
			D3D11_SUBRESOURCE_DATA subResourceData{};
			subResourceData.pSysMem = &shaderVariables;

			//Create shader variables buffer
			hResult = vDirectXInstance.iD3D11Device5->CreateBuffer(&bufferDescription, &subResourceData, &vDirectXInstance.iD3D11BufferPixel0);
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"Failed creating shader variables buffer") };
			}

			//Set shader variables
			vDirectXInstance.iD3D11DeviceContext4->PSSetConstantBuffers(0, 1, &vDirectXInstance.iD3D11BufferPixel0);

			//Return result
			return { .Status = CaptureStatus::Success };
		}
		catch (...)
		{
			//Return result
			return { .Status = CaptureStatus::Failed, .Message = SysAllocString(L"SetShaderVariables failed") };
		}
	}

	CaptureResult CaptureInitializeCode(CaptureSettings captureSettings, BOOL forceInitialize)
	{
		try
		{
			//Check instance status
			if (vCaptureInstance.vInstanceInitializing)
			{
				//Return result
				AVDebugWriteLine("Capture is currently initializing.");
				return { .Status = CaptureStatus::Busy, .Message = SysAllocString(L"Currently initializing") };
			}
			else if (!forceInitialize && vCaptureInstance.vInstanceInitialized)
			{
				//Return result
				AVDebugWriteLine("Capture is already initialized.");
				return { .Status = CaptureStatus::Success, .Message = SysAllocString(L"Already initialized") };
			}

			//Update instance status
			vCaptureInstance.vInstanceInitializing = true;

			AVDebugWriteLine("Initializing capture...");

			//Disable assert reporting
			_CrtSetReportMode(_CRT_ASSERT, 0);

			//Set process dpi awareness
			SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);

			//Reset all used variables
			ResetVariablesAll();

			//Update capture settings
			vCaptureSettings = captureSettings;

			//Initialize DirectX
			capResult = InitializeDirectXLoop(vCaptureSettings.MonitorId);
			if (capResult.Status != CaptureStatus::Success)
			{
				//Reset all used variables
				ResetVariablesAll();

				//Play audio effect
				PlayAudio(L"Assets\\Capture\\CaptureFailed.mp3");

				//Return result
				return capResult;
			}

			//Apply DirectX performance tweaks
			capResult = InitializeDirectXTweaks();
			if (capResult.Status != CaptureStatus::Success)
			{
				//Reset all used variables
				ResetVariablesAll();

				//Play audio effect
				PlayAudio(L"Assets\\Capture\\CaptureFailed.mp3");

				//Return result
				return capResult;
			}

			//Set capture details
			capResult = SetCaptureDetails();
			if (capResult.Status != CaptureStatus::Success)
			{
				//Reset all used variables
				ResetVariablesAll();

				//Play audio effect
				PlayAudio(L"Assets\\Capture\\CaptureFailed.mp3");

				//Return result
				return capResult;
			}

			//Initialize sampler state
			capResult = InitializeSamplerState();
			if (capResult.Status != CaptureStatus::Success)
			{
				//Reset all used variables
				ResetVariablesAll();

				//Play audio effect
				PlayAudio(L"Assets\\Capture\\CaptureFailed.mp3");

				//Return result
				return capResult;
			}

			//Initialize Windows Graphics Capture
			capResult = InitializeWgc();
			if (capResult.Status != CaptureStatus::Success)
			{
				//Reset all used variables
				ResetVariablesAll();

				//Play audio effect
				PlayAudio(L"Assets\\Capture\\CaptureFailed.mp3");

				//Return result
				return capResult;
			}

			//Initialize shaders
			capResult = InitializeShaders();
			if (capResult.Status != CaptureStatus::Success)
			{
				//Reset all used variables
				ResetVariablesAll();

				//Play audio effect
				PlayAudio(L"Assets\\Capture\\CaptureFailed.mp3");

				//Return result
				return capResult;
			}

			//Initialize render target view
			capResult = InitializeRenderTargetView();
			if (capResult.Status != CaptureStatus::Success)
			{
				//Reset all used variables
				ResetVariablesAll();

				//Play audio effect
				PlayAudio(L"Assets\\Capture\\CaptureFailed.mp3");

				//Return result
				return capResult;
			}

			//Initialize view port
			capResult = InitializeViewPort();
			if (capResult.Status != CaptureStatus::Success)
			{
				//Reset all used variables
				ResetVariablesAll();

				//Play audio effect
				PlayAudio(L"Assets\\Capture\\CaptureFailed.mp3");

				//Return result
				return capResult;
			}

			//Set shader variables
			capResult = SetShaderVariables();
			if (capResult.Status != CaptureStatus::Success)
			{
				//Reset all used variables
				ResetVariablesAll();

				//Play audio effect
				PlayAudio(L"Assets\\Capture\\CaptureFailed.mp3");

				//Return result
				return capResult;
			}

			//Update vertex vertices
			capResult = RenderUpdateVertex(VertexVerticesArrayScreen, VertexVerticesCountScreen);
			if (capResult.Status != CaptureStatus::Success)
			{
				//Reset all used variables
				ResetVariablesAll();

				//Play audio effect
				PlayAudio(L"Assets\\Capture\\CaptureFailed.mp3");

				//Return result
				return capResult;
			}

			//Update thread variables
			vCaptureInstance.vCaptureStatusLoopAllowed = true;
			vCaptureInstance.vCaptureStatusLoopFinished = false;

			//Loop capture status check
			std::thread threadLoopCaptureStatus(LoopCaptureStatus);
			threadLoopCaptureStatus.detach();

			//Wait for capture initialization
			AVHighResDelay(100);

			//Update instance status
			vCaptureInstance.vInstanceInitialized = true;
			vCaptureInstance.vInstanceInitializing = false;

			//Return result
			AVDebugWriteLine("Capture initialized successfully.");
			return { .Status = CaptureStatus::Success };
		}
		catch (...)
		{
			//Reset all used variables
			ResetVariablesAll();

			//Play audio effect
			PlayAudio(L"Assets\\Capture\\CaptureFailed.mp3");

			//Return result
			return { .Status = CaptureStatus::Failed, .Message = SysAllocString(L"CaptureInitializeCode failed") };
		}
	}
}