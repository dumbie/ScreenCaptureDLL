#pragma once
#include "CaptureVariables.h"
#include "CaptureReset.cpp"
#include "CaptureDetails.cpp"

namespace
{
	BOOL InitializeDirectX(UINT captureInstanceId)
	{
		try
		{
			//Create D3D11 Device
			D3D_FEATURE_LEVEL iD3DFeatureLevel;
			UINT iD3DCreateFlags = D3D11_CREATE_DEVICE_VIDEO_SUPPORT;
			hResult = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, iD3DCreateFlags, D3DFeatureLevelsArray, D3DFeatureLevelsCount, D3D11_SDK_VERSION, &vCaptureInstances[captureInstanceId].iD3D11Device0, &iD3DFeatureLevel, &vCaptureInstances[captureInstanceId].iD3D11DeviceContext0);
			if (FAILED(hResult))
			{
				CaptureResetVariablesAll(captureInstanceId);
				return false;
			}

			//Convert variables
			hResult = vCaptureInstances[captureInstanceId].iD3D11Device0->QueryInterface(&vCaptureInstances[captureInstanceId].iD3D11Device5);
			if (FAILED(hResult))
			{
				CaptureResetVariablesAll(captureInstanceId);
				return false;
			}

			//Convert variables
			hResult = vCaptureInstances[captureInstanceId].iD3D11Device5->QueryInterface(&vCaptureInstances[captureInstanceId].iDxgiDevice4);
			if (FAILED(hResult))
			{
				CaptureResetVariablesAll(captureInstanceId);
				return false;
			}

			//Convert variables
			hResult = vCaptureInstances[captureInstanceId].iD3D11Device5->QueryInterface(&vCaptureInstances[captureInstanceId].iD3D11Multithread);
			if (FAILED(hResult))
			{
				CaptureResetVariablesAll(captureInstanceId);
				return false;
			}

			//Convert variables
			hResult = vCaptureInstances[captureInstanceId].iD3D11DeviceContext0->QueryInterface(&vCaptureInstances[captureInstanceId].iD3D11DeviceContext4);
			if (FAILED(hResult))
			{
				CaptureResetVariablesAll(captureInstanceId);
				return false;
			}

			//Set multithread protected
			hResult = vCaptureInstances[captureInstanceId].iD3D11Multithread->SetMultithreadProtected(true);
			if (FAILED(hResult))
			{
				CaptureResetVariablesAll(captureInstanceId);
				return false;
			}

			//Get DXGI Adapter
			hResult = vCaptureInstances[captureInstanceId].iDxgiDevice4->GetParent(IID_PPV_ARGS(&vCaptureInstances[captureInstanceId].iDxgiAdapter4));
			if (FAILED(hResult))
			{
				CaptureResetVariablesAll(captureInstanceId);
				return false;
			}

			//Get DXGI Output
			hResult = vCaptureInstances[captureInstanceId].iDxgiAdapter4->EnumOutputs(vCaptureInstances[captureInstanceId].vCaptureSettings.MonitorId, &vCaptureInstances[captureInstanceId].iDxgiOutput0);
			if (FAILED(hResult))
			{
				CaptureResetVariablesAll(captureInstanceId);
				return false;
			}

			//Convert variables
			hResult = vCaptureInstances[captureInstanceId].iDxgiOutput0->QueryInterface(&vCaptureInstances[captureInstanceId].iDxgiOutput6);
			if (FAILED(hResult))
			{
				CaptureResetVariablesAll(captureInstanceId);
				return false;
			}

			//Get output duplicate (requires process dpi awareness) (only one per process allowed)
			hResult = vCaptureInstances[captureInstanceId].iDxgiOutput6->DuplicateOutput1(vCaptureInstances[captureInstanceId].iD3D11Device5, 0, iDxgiFormatsCount, iDxgiFormatsArray, &vCaptureInstances[captureInstanceId].iDxgiOutputDuplication0);
			if (FAILED(hResult))
			{
				std::cout << "DuplicateOutput failed: " << hResult << std::endl;
				CaptureResetVariablesAll(captureInstanceId);
				return false;
			}

			//Get output description
			DXGI_OUTPUT_DESC1 iDxgiOutputDescription;
			hResult = vCaptureInstances[captureInstanceId].iDxgiOutput6->GetDesc1(&iDxgiOutputDescription);
			if (FAILED(hResult))
			{
				CaptureResetVariablesAll(captureInstanceId);
				return false;
			}

			//Get duplicate description
			DXGI_OUTDUPL_DESC iDxgiOutputDuplicationDescription;
			vCaptureInstances[captureInstanceId].iDxgiOutputDuplication0->GetDesc(&iDxgiOutputDuplicationDescription);

			//Get and set HDR details
			vCaptureInstances[captureInstanceId].vCaptureDetails.HDREnabled = iDxgiOutputDescription.ColorSpace == DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020 || iDxgiOutputDescription.ColorSpace == DXGI_COLOR_SPACE_RGB_STUDIO_G2084_NONE_P2020;
			if (vCaptureInstances[captureInstanceId].vCaptureDetails.HDREnabled)
			{
				vCaptureInstances[captureInstanceId].vCaptureDetails.HDRtoSDR = vCaptureInstances[captureInstanceId].vCaptureSettings.HDRtoSDR;
				if (vCaptureInstances[captureInstanceId].vCaptureDetails.HDRtoSDR)
				{
					vCaptureInstances[captureInstanceId].iWicPixelFormatGuidSource = GUID_WICPixelFormat32bppBGRA;
					vCaptureInstances[captureInstanceId].vCaptureDxgiFormat = DXGI_FORMAT_B8G8R8A8_UNORM; //DXGI_FORMAT_B8G8R8A8_UNORM_SRGB
					vCaptureInstances[captureInstanceId].vCaptureDetails.PixelByteSize = 4;
					vCaptureInstances[captureInstanceId].vCaptureDetails.SDRWhiteLevel = GetMonitorSDRWhiteLevel(captureInstanceId);
				}
				else
				{
					vCaptureInstances[captureInstanceId].iWicPixelFormatGuidSource = GUID_WICPixelFormat64bppRGBAHalf;
					vCaptureInstances[captureInstanceId].vCaptureDxgiFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
					vCaptureInstances[captureInstanceId].vCaptureDetails.PixelByteSize = 8;
				}
			}
			else
			{
				vCaptureInstances[captureInstanceId].vCaptureDetails.HDRtoSDR = false;
				vCaptureInstances[captureInstanceId].iWicPixelFormatGuidSource = GUID_WICPixelFormat32bppBGRA;
				vCaptureInstances[captureInstanceId].vCaptureDxgiFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
				vCaptureInstances[captureInstanceId].vCaptureDetails.PixelByteSize = 4;
			}

			//Update capture variables
			vCaptureInstances[captureInstanceId].vCaptureDetails.OriginalWidth = iDxgiOutputDuplicationDescription.ModeDesc.Width;
			vCaptureInstances[captureInstanceId].vCaptureDetails.OriginalHeight = iDxgiOutputDuplicationDescription.ModeDesc.Height;
			vCaptureInstances[captureInstanceId].vCaptureDetails.RefreshRate = iDxgiOutputDuplicationDescription.ModeDesc.RefreshRate.Numerator;
			vCaptureInstances[captureInstanceId].vCaptureTextureResizing = vCaptureInstances[captureInstanceId].vCaptureSettings.MaxPixelDimension != 0 && vCaptureInstances[captureInstanceId].vCaptureDetails.OriginalWidth > vCaptureInstances[captureInstanceId].vCaptureSettings.MaxPixelDimension && vCaptureInstances[captureInstanceId].vCaptureDetails.OriginalHeight > vCaptureInstances[captureInstanceId].vCaptureSettings.MaxPixelDimension;
			if (vCaptureInstances[captureInstanceId].vCaptureTextureResizing)
			{
				DOUBLE resizedWidth = 0.01;
				DOUBLE resizedHeight = 0.01;
				UINT minDimension = min(vCaptureInstances[captureInstanceId].vCaptureDetails.OriginalWidth, vCaptureInstances[captureInstanceId].vCaptureDetails.OriginalHeight);
				//Find nearest full pixel dimensions to keep ratio
				while (resizedWidth != (UINT)resizedWidth || resizedHeight != (UINT)resizedHeight)
				{
					DOUBLE differenceRatio = (DOUBLE)minDimension / vCaptureInstances[captureInstanceId].vCaptureSettings.MaxPixelDimension;
					resizedWidth = vCaptureInstances[captureInstanceId].vCaptureDetails.OriginalWidth / differenceRatio;
					resizedHeight = vCaptureInstances[captureInstanceId].vCaptureDetails.OriginalHeight / differenceRatio;
					vCaptureInstances[captureInstanceId].vCaptureSettings.MaxPixelDimension++;
				}
				vCaptureInstances[captureInstanceId].vCaptureDetails.OutputWidth = resizedWidth;
				vCaptureInstances[captureInstanceId].vCaptureDetails.OutputHeight = resizedHeight;
			}
			else
			{
				vCaptureInstances[captureInstanceId].vCaptureDetails.OutputWidth = vCaptureInstances[captureInstanceId].vCaptureDetails.OriginalWidth;
				vCaptureInstances[captureInstanceId].vCaptureDetails.OutputHeight = vCaptureInstances[captureInstanceId].vCaptureDetails.OriginalHeight;
			}
			std::cout << "Screen capture output, Width: " << vCaptureInstances[captureInstanceId].vCaptureDetails.OutputWidth << " Height: " << vCaptureInstances[captureInstanceId].vCaptureDetails.OutputHeight << std::endl;
			vCaptureInstances[captureInstanceId].vCaptureDetails.WidthByteSize = vCaptureInstances[captureInstanceId].vCaptureDetails.OutputWidth * vCaptureInstances[captureInstanceId].vCaptureDetails.PixelByteSize;
			vCaptureInstances[captureInstanceId].vCaptureDetails.TotalByteSize = vCaptureInstances[captureInstanceId].vCaptureDetails.OutputWidth * vCaptureInstances[captureInstanceId].vCaptureDetails.OutputHeight * vCaptureInstances[captureInstanceId].vCaptureDetails.PixelByteSize;
			vCaptureInstances[captureInstanceId].vCaptureTextureMipLevels = 1 + log2(max(vCaptureInstances[captureInstanceId].vCaptureDetails.OutputWidth, vCaptureInstances[captureInstanceId].vCaptureDetails.OutputHeight));

			//Release resources
			vCaptureInstances[captureInstanceId].iD3D11Device0.Release();
			vCaptureInstances[captureInstanceId].iD3D11DeviceContext0.Release();
			vCaptureInstances[captureInstanceId].iDxgiDevice4.Release();
			vCaptureInstances[captureInstanceId].iDxgiAdapter4.Release();
			vCaptureInstances[captureInstanceId].iDxgiOutput0.Release();
			vCaptureInstances[captureInstanceId].iDxgiOutput6.Release();

			return true;
		}
		catch (...)
		{
			CaptureResetVariablesAll(captureInstanceId);
			std::cout << "InitializeDirectX failed: " << hResult << std::endl;
			return false;
		}
	}

	BOOL InitializeSamplerState(UINT captureInstanceId)
	{
		try
		{
			//Create sampler description
			D3D11_SAMPLER_DESC iD3DSamplerDescription{};
			iD3DSamplerDescription.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
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
			hResult = vCaptureInstances[captureInstanceId].iD3D11Device5->CreateSamplerState(&iD3DSamplerDescription, &vCaptureInstances[captureInstanceId].iD3D11SamplerState0);
			if (FAILED(hResult))
			{
				return false;
			}

			//Set sampler state
			vCaptureInstances[captureInstanceId].iD3D11DeviceContext4->PSSetSamplers(0, 1, &vCaptureInstances[captureInstanceId].iD3D11SamplerState0);

			//Release resources
			vCaptureInstances[captureInstanceId].iD3D11SamplerState0.Release();

			return true;
		}
		catch (...)
		{
			std::cout << "InitializeSamplerState failed: " << hResult << std::endl;
			return false;
		}
	}

	BOOL InitializeRenderTargetView(UINT captureInstanceId)
	{
		try
		{
			//Create render target view texture
			D3D11_TEXTURE2D_DESC iD3DTexture2D0DescRenderTargetView{};
			iD3DTexture2D0DescRenderTargetView.Width = vCaptureInstances[captureInstanceId].vCaptureDetails.OutputWidth;
			iD3DTexture2D0DescRenderTargetView.Height = vCaptureInstances[captureInstanceId].vCaptureDetails.OutputHeight;
			iD3DTexture2D0DescRenderTargetView.MipLevels = 1;
			iD3DTexture2D0DescRenderTargetView.ArraySize = 1;
			iD3DTexture2D0DescRenderTargetView.Format = vCaptureInstances[captureInstanceId].vCaptureDxgiFormat;
			iD3DTexture2D0DescRenderTargetView.SampleDesc.Count = 1;
			iD3DTexture2D0DescRenderTargetView.SampleDesc.Quality = 0;
			iD3DTexture2D0DescRenderTargetView.Usage = D3D11_USAGE_DEFAULT;
			iD3DTexture2D0DescRenderTargetView.BindFlags = D3D11_BIND_RENDER_TARGET;
			iD3DTexture2D0DescRenderTargetView.CPUAccessFlags = 0;
			iD3DTexture2D0DescRenderTargetView.MiscFlags = 0;
			hResult = vCaptureInstances[captureInstanceId].iD3D11Device5->CreateTexture2D(&iD3DTexture2D0DescRenderTargetView, NULL, &vCaptureInstances[captureInstanceId].iD3D11Texture2D0RenderTargetView);
			if (FAILED(hResult))
			{
				return false;
			}

			//Create and set render target view
			hResult = vCaptureInstances[captureInstanceId].iD3D11Device5->CreateRenderTargetView(vCaptureInstances[captureInstanceId].iD3D11Texture2D0RenderTargetView, NULL, &vCaptureInstances[captureInstanceId].iD3D11RenderTargetView0);
			if (FAILED(hResult))
			{
				return false;
			}
			vCaptureInstances[captureInstanceId].iD3D11DeviceContext4->OMSetRenderTargets(1, &vCaptureInstances[captureInstanceId].iD3D11RenderTargetView0, NULL);
			vCaptureInstances[captureInstanceId].iD3D11DeviceContext4->ClearRenderTargetView(vCaptureInstances[captureInstanceId].iD3D11RenderTargetView0, ColorRgbaBlack);

			//Release resources
			vCaptureInstances[captureInstanceId].iD3D11RenderTargetView0.Release();

			return true;
		}
		catch (...)
		{
			std::cout << "InitializeRenderTargetView failed: " << hResult << std::endl;
			return false;
		}
	}

	BOOL InitializeViewPort(UINT captureInstanceId)
	{
		try
		{
			//Create and set viewport
			D3D11_VIEWPORT iD3D11ViewPort{};
			iD3D11ViewPort.Width = vCaptureInstances[captureInstanceId].vCaptureDetails.OutputWidth;
			iD3D11ViewPort.Height = vCaptureInstances[captureInstanceId].vCaptureDetails.OutputHeight;
			vCaptureInstances[captureInstanceId].iD3D11DeviceContext4->RSSetViewports(1, &iD3D11ViewPort);

			return true;
		}
		catch (...)
		{
			std::cout << "InitializeViewPort failed: " << hResult << std::endl;
			return false;
		}
	}

	BOOL InitializeShaders(UINT captureInstanceId)
	{
		try
		{
			//Load shaders from file
			hResult = D3DCompileFromFile(L"Resources\\VertexShader.hlsl", 0, 0, "main", "vs_5_0", 0, 0, &vCaptureInstances[captureInstanceId].iD3DBlob0VertexShader, 0);
			if (FAILED(hResult))
			{
				return false;
			}
			hResult = D3DCompileFromFile(L"Resources\\PixelShader.hlsl", 0, 0, "main", "ps_5_0", 0, 0, &vCaptureInstances[captureInstanceId].iD3DBlob0PixelShader, 0);
			if (FAILED(hResult))
			{
				return false;
			}

			//Create shaders from blob
			hResult = vCaptureInstances[captureInstanceId].iD3D11Device5->CreateVertexShader(vCaptureInstances[captureInstanceId].iD3DBlob0VertexShader->GetBufferPointer(), vCaptureInstances[captureInstanceId].iD3DBlob0VertexShader->GetBufferSize(), NULL, &vCaptureInstances[captureInstanceId].iD3D11VertexShader0);
			if (FAILED(hResult))
			{
				return false;
			}
			hResult = vCaptureInstances[captureInstanceId].iD3D11Device5->CreatePixelShader(vCaptureInstances[captureInstanceId].iD3DBlob0PixelShader->GetBufferPointer(), vCaptureInstances[captureInstanceId].iD3DBlob0PixelShader->GetBufferSize(), NULL, &vCaptureInstances[captureInstanceId].iD3D11PixelShader0);
			if (FAILED(hResult))
			{
				return false;
			}

			//Create and set input layout
			hResult = vCaptureInstances[captureInstanceId].iD3D11Device5->CreateInputLayout(InputElementsArray, InputElementsCount, vCaptureInstances[captureInstanceId].iD3DBlob0VertexShader->GetBufferPointer(), vCaptureInstances[captureInstanceId].iD3DBlob0VertexShader->GetBufferSize(), &vCaptureInstances[captureInstanceId].iD3D11InputLayout0);
			if (FAILED(hResult))
			{
				return false;
			}
			vCaptureInstances[captureInstanceId].iD3D11DeviceContext4->IASetInputLayout(vCaptureInstances[captureInstanceId].iD3D11InputLayout0);

			//Set shaders
			vCaptureInstances[captureInstanceId].iD3D11DeviceContext4->VSSetShader(vCaptureInstances[captureInstanceId].iD3D11VertexShader0, NULL, 0);
			vCaptureInstances[captureInstanceId].iD3D11DeviceContext4->PSSetShader(vCaptureInstances[captureInstanceId].iD3D11PixelShader0, NULL, 0);

			//Release resources
			vCaptureInstances[captureInstanceId].iD3D11Buffer0.Release();
			vCaptureInstances[captureInstanceId].iD3DBlob0VertexShader.Release();
			vCaptureInstances[captureInstanceId].iD3DBlob0PixelShader.Release();
			vCaptureInstances[captureInstanceId].iD3D11VertexShader0.Release();
			vCaptureInstances[captureInstanceId].iD3D11PixelShader0.Release();
			vCaptureInstances[captureInstanceId].iD3D11InputLayout0.Release();

			return true;
		}
		catch (...)
		{
			std::cout << "InitializeShaders failed: " << hResult << std::endl;
			return false;
		}
	}

	BOOL SetShaderVariables(UINT captureInstanceId)
	{
		try
		{
			//Check if DirectX is initialized
			if (vCaptureInstances[captureInstanceId].iD3D11Device5 == NULL || vCaptureInstances[captureInstanceId].iD3D11DeviceContext4 == NULL)
			{
				return false;
			}

			//Create shader variables
			ShaderVariables shaderVariables{};
			shaderVariables.HDRtoSDR = vCaptureInstances[captureInstanceId].vCaptureDetails.HDRtoSDR;
			shaderVariables.HDRPaperWhite = vCaptureInstances[captureInstanceId].vCaptureSettings.HDRPaperWhite;
			shaderVariables.HDRMaximumNits = vCaptureInstances[captureInstanceId].vCaptureSettings.HDRMaximumNits;
			shaderVariables.SDRWhiteLevel = vCaptureInstances[captureInstanceId].vCaptureDetails.SDRWhiteLevel;
			shaderVariables.Saturation = vCaptureInstances[captureInstanceId].vCaptureSettings.Saturation;
			shaderVariables.RedChannel = vCaptureInstances[captureInstanceId].vCaptureSettings.RedChannel;
			shaderVariables.GreenChannel = vCaptureInstances[captureInstanceId].vCaptureSettings.GreenChannel;
			shaderVariables.BlueChannel = vCaptureInstances[captureInstanceId].vCaptureSettings.BlueChannel;
			shaderVariables.Brightness = vCaptureInstances[captureInstanceId].vCaptureSettings.Brightness;
			shaderVariables.Contrast = vCaptureInstances[captureInstanceId].vCaptureSettings.Contrast;
			shaderVariables.Gamma = vCaptureInstances[captureInstanceId].vCaptureSettings.Gamma;
			shaderVariables.Blur = vCaptureInstances[captureInstanceId].vCaptureSettings.Blur;

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
			hResult = vCaptureInstances[captureInstanceId].iD3D11Device5->CreateBuffer(&bufferDescription, &subResourceData, &vCaptureInstances[captureInstanceId].iD3D11Buffer0);
			if (FAILED(hResult))
			{
				return false;
			}

			//Set shader variables
			vCaptureInstances[captureInstanceId].iD3D11DeviceContext4->PSSetConstantBuffers(0, 1, &vCaptureInstances[captureInstanceId].iD3D11Buffer0);

			//Release resources
			vCaptureInstances[captureInstanceId].iD3D11Buffer0.Release();

			return true;
		}
		catch (...)
		{
			std::cout << "SetShaderVariables failed: " << hResult << std::endl;
			return false;
		}
	}

	BOOL InitializeCapture(UINT captureInstanceId, CaptureSettings captureSettings, CaptureDetails& captureDetails)
	{
		try
		{
			//Check capture initialized
			if (vCaptureInstances[captureInstanceId].vCaptureDeviceInitialized)
			{
				//Return capture details
				captureDetails = vCaptureInstances[captureInstanceId].vCaptureDetails;

				//Return result
				std::cout << "Capture is already initialized." << std::endl;
				return true;
			}

			//Disable assert reporting
			_CrtSetReportMode(_CRT_ASSERT, 0);

			//Set process dpi awareness
			SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);

			//Reset all used variables
			CaptureResetVariablesAll(captureInstanceId);

			//Update capture settings
			vCaptureInstances[captureInstanceId].vCaptureSettings = captureSettings;

			//Initialize DirectX
			if (!InitializeDirectX(captureInstanceId)) { return false; }

			//Initialize sampler state
			if (!InitializeSamplerState(captureInstanceId)) { return false; }

			//Initialize render target view
			if (!InitializeRenderTargetView(captureInstanceId)) { return false; }

			//Initialize view port
			if (!InitializeViewPort(captureInstanceId)) { return false; }

			//Initialize shaders
			if (!InitializeShaders(captureInstanceId)) { return false; }

			//Set shader variables
			if (!SetShaderVariables(captureInstanceId)) { return false; }

			//Update variables
			vCaptureInstances[captureInstanceId].vCaptureDeviceInitialized = true;

			//Return capture details
			captureDetails = vCaptureInstances[captureInstanceId].vCaptureDetails;

			//Return result
			std::cout << "Capture initialized successfully for instance: " << captureInstanceId << std::endl;
			return true;
		}
		catch (...)
		{
			std::cout << "InitializeCapture failed: " << hResult << std::endl;
			return false;
		}
	}
}