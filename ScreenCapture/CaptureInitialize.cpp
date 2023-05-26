#pragma once
#include "CaptureVariables.h"
#include "CaptureReset.cpp"
#include "CaptureDetails.cpp"

namespace
{
	BOOL InitializeDirectX()
	{
		try
		{
			//Create D3D11 Device
			D3D_FEATURE_LEVEL iD3DFeatureLevel;
			UINT iD3DCreateFlags = D3D11_CREATE_DEVICE_VIDEO_SUPPORT;
			hResult = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, iD3DCreateFlags, D3DFeatureLevelsArray, D3DFeatureLevelsCount, D3D11_SDK_VERSION, &iD3D11Device0, &iD3DFeatureLevel, &iD3D11DeviceContext0);
			if (FAILED(hResult))
			{
				CaptureResetVariablesAll();
				return false;
			}

			//Convert variables
			hResult = iD3D11Device0->QueryInterface(&iD3D11Device5);
			if (FAILED(hResult))
			{
				CaptureResetVariablesAll();
				return false;
			}

			//Convert variables
			hResult = iD3D11Device5->QueryInterface(&iDxgiDevice4);
			if (FAILED(hResult))
			{
				CaptureResetVariablesAll();
				return false;
			}

			//Convert variables
			hResult = iD3D11Device5->QueryInterface(&iD3D11Multithread);
			if (FAILED(hResult))
			{
				CaptureResetVariablesAll();
				return false;
			}

			//Convert variables
			hResult = iD3D11DeviceContext0->QueryInterface(&iD3D11DeviceContext4);
			if (FAILED(hResult))
			{
				CaptureResetVariablesAll();
				return false;
			}

			//Set multithread protected
			hResult = iD3D11Multithread->SetMultithreadProtected(true);
			if (FAILED(hResult))
			{
				CaptureResetVariablesAll();
				return false;
			}

			//Get DXGI Adapter
			hResult = iDxgiDevice4->GetParent(IID_PPV_ARGS(&iDxgiAdapter4));
			if (FAILED(hResult))
			{
				CaptureResetVariablesAll();
				return false;
			}

			//Get DXGI Output
			hResult = iDxgiAdapter4->EnumOutputs(vCaptureSettings.MonitorId, &iDxgiOutput0);
			if (FAILED(hResult))
			{
				CaptureResetVariablesAll();
				return false;
			}

			//Convert variables
			hResult = iDxgiOutput0->QueryInterface(&iDxgiOutput6);
			if (FAILED(hResult))
			{
				CaptureResetVariablesAll();
				return false;
			}

			//Get output duplicate (requires process dpi awareness)
			hResult = iDxgiOutput6->DuplicateOutput1(iD3D11Device5, 0, iDxgiFormatsCount, iDxgiFormatsArray, &iDxgiOutputDuplication0);
			if (FAILED(hResult))
			{
				CaptureResetVariablesAll();
				return false;
			}

			//Get output description
			DXGI_OUTPUT_DESC1 iDxgiOutputDescription;
			hResult = iDxgiOutput6->GetDesc1(&iDxgiOutputDescription);
			if (FAILED(hResult))
			{
				CaptureResetVariablesAll();
				return false;
			}

			//Get duplicate description
			DXGI_OUTDUPL_DESC iDxgiOutputDuplicationDescription;
			iDxgiOutputDuplication0->GetDesc(&iDxgiOutputDuplicationDescription);

			//Get monitor HDR details
			vCaptureDetails.HDREnabled = iDxgiOutputDescription.ColorSpace == DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020 || iDxgiOutputDescription.ColorSpace == DXGI_COLOR_SPACE_RGB_STUDIO_G2084_NONE_P2020;
			if (vCaptureDetails.HDREnabled)
			{
				if (vCaptureSettings.HDRtoSDR)
				{
					iWicPixelFormatGuidSource = GUID_WICPixelFormat32bppBGRA;
					vCaptureDxgiFormat = DXGI_FORMAT_B8G8R8A8_UNORM; //DXGI_FORMAT_B8G8R8A8_UNORM_SRGB
					vCaptureDetails.PixelByteSize = 4;
					vCaptureDetails.SDRWhiteLevel = GetMonitorSDRWhiteLevel();
				}
				else
				{
					iWicPixelFormatGuidSource = GUID_WICPixelFormat64bppRGBAHalf;
					vCaptureDxgiFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
					vCaptureDetails.PixelByteSize = 8;
				}
			}
			else
			{
				iWicPixelFormatGuidSource = GUID_WICPixelFormat32bppBGRA;
				vCaptureDxgiFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
				vCaptureDetails.PixelByteSize = 4;
			}

			//Update capture variables
			vCaptureDetails.OriginalWidth = iDxgiOutputDuplicationDescription.ModeDesc.Width;
			vCaptureDetails.OriginalHeight = iDxgiOutputDuplicationDescription.ModeDesc.Height;
			vCaptureDetails.RefreshRate = iDxgiOutputDuplicationDescription.ModeDesc.RefreshRate.Numerator;
			vCaptureTextureResizing = vCaptureSettings.MaxPixelDimension != 0 && vCaptureDetails.OriginalWidth > vCaptureSettings.MaxPixelDimension && vCaptureDetails.OriginalHeight > vCaptureSettings.MaxPixelDimension;
			if (vCaptureTextureResizing)
			{
				DOUBLE resizedWidth = 0.01;
				DOUBLE resizedHeight = 0.01;
				UINT minDimension = min(vCaptureDetails.OriginalWidth, vCaptureDetails.OriginalHeight);
				//Find nearest full pixel dimensions to keep ratio
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
			vCaptureTextureMipLevels = 1 + log2(max(vCaptureDetails.OutputWidth, vCaptureDetails.OutputHeight));

			//Release resources
			iD3D11Device0.Release();
			iD3D11DeviceContext0.Release();
			iDxgiDevice4.Release();
			iDxgiAdapter4.Release();
			iDxgiOutput0.Release();
			iDxgiOutput6.Release();

			return true;
		}
		catch (...)
		{
			CaptureResetVariablesAll();
			std::cout << "InitializeDirectX failed: " << hResult << std::endl;
			return false;
		}
	}

	BOOL InitializeSamplerState()
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
			hResult = iD3D11Device5->CreateSamplerState(&iD3DSamplerDescription, &iD3D11SamplerState0);
			if (FAILED(hResult))
			{
				return false;
			}

			//Set sampler state
			iD3D11DeviceContext4->PSSetSamplers(0, 1, &iD3D11SamplerState0);

			//Release resources
			iD3D11SamplerState0.Release();

			return true;
		}
		catch (...)
		{
			std::cout << "InitializeSamplerState failed: " << hResult << std::endl;
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
			iD3DTexture2D0DescRenderTargetView.Format = vCaptureDxgiFormat;
			iD3DTexture2D0DescRenderTargetView.SampleDesc.Count = 1;
			iD3DTexture2D0DescRenderTargetView.SampleDesc.Quality = 0;
			iD3DTexture2D0DescRenderTargetView.Usage = D3D11_USAGE_DEFAULT;
			iD3DTexture2D0DescRenderTargetView.BindFlags = D3D11_BIND_RENDER_TARGET;
			iD3DTexture2D0DescRenderTargetView.CPUAccessFlags = 0;
			iD3DTexture2D0DescRenderTargetView.MiscFlags = 0;
			hResult = iD3D11Device5->CreateTexture2D(&iD3DTexture2D0DescRenderTargetView, NULL, &iD3D11Texture2D0RenderTargetView);
			if (FAILED(hResult))
			{
				return false;
			}

			//Create and set render target view
			hResult = iD3D11Device5->CreateRenderTargetView(iD3D11Texture2D0RenderTargetView, NULL, &iD3D11RenderTargetView0);
			if (FAILED(hResult))
			{
				return false;
			}
			iD3D11DeviceContext4->OMSetRenderTargets(1, &iD3D11RenderTargetView0, NULL);
			iD3D11DeviceContext4->ClearRenderTargetView(iD3D11RenderTargetView0, ColorRgbaBlack);

			//Release resources
			iD3D11RenderTargetView0.Release();

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
			iD3D11DeviceContext4->RSSetViewports(1, &iD3D11ViewPort);

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
			hResult = D3DCompileFromFile(L"Resources\\VertexShader.hlsl", 0, 0, "main", "vs_5_0", 0, 0, &iD3DBlob0VertexShader, 0);
			if (FAILED(hResult))
			{
				return false;
			}
			hResult = D3DCompileFromFile(L"Resources\\PixelShader.hlsl", 0, 0, "main", "ps_5_0", 0, 0, &iD3DBlob0PixelShader, 0);
			if (FAILED(hResult))
			{
				return false;
			}

			//Create shaders from blob
			hResult = iD3D11Device5->CreateVertexShader(iD3DBlob0VertexShader->GetBufferPointer(), iD3DBlob0VertexShader->GetBufferSize(), NULL, &iD3D11VertexShader0);
			if (FAILED(hResult))
			{
				return false;
			}
			hResult = iD3D11Device5->CreatePixelShader(iD3DBlob0PixelShader->GetBufferPointer(), iD3DBlob0PixelShader->GetBufferSize(), NULL, &iD3D11PixelShader0);
			if (FAILED(hResult))
			{
				return false;
			}

			//Create and set input layout
			hResult = iD3D11Device5->CreateInputLayout(InputElementsArray, InputElementsCount, iD3DBlob0VertexShader->GetBufferPointer(), iD3DBlob0VertexShader->GetBufferSize(), &iD3D11InputLayout0);
			if (FAILED(hResult))
			{
				return false;
			}
			iD3D11DeviceContext4->IASetInputLayout(iD3D11InputLayout0);

			//Set shaders
			iD3D11DeviceContext4->VSSetShader(iD3D11VertexShader0, NULL, 0);
			iD3D11DeviceContext4->PSSetShader(iD3D11PixelShader0, NULL, 0);

			//Release resources
			iD3D11Buffer0.Release();
			iD3DBlob0VertexShader.Release();
			iD3DBlob0PixelShader.Release();
			iD3D11VertexShader0.Release();
			iD3D11PixelShader0.Release();
			iD3D11InputLayout0.Release();

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
			//Check if DirectX is initialized
			if (iD3D11Device5 == NULL || iD3D11DeviceContext4 == NULL) { return false; }

			//Create shader variables
			ShaderVariables shaderVariables{};
			shaderVariables.HDRtoSDR = vCaptureDetails.HDREnabled & vCaptureSettings.HDRtoSDR;
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
			hResult = iD3D11Device5->CreateBuffer(&bufferDescription, &subResourceData, &iD3D11Buffer0);
			if (FAILED(hResult))
			{
				return false;
			}

			//Set shader variables
			iD3D11DeviceContext4->PSSetConstantBuffers(0, 1, &iD3D11Buffer0);

			//Release resources
			iD3D11Buffer0.Release();

			return true;
		}
		catch (...)
		{
			std::cout << "SetShaderVariables failed: " << hResult << std::endl;
			return false;
		}
	}

	BOOL InitializeCapture(CaptureSettings captureSettings, CaptureDetails& captureDetails)
	{
		try
		{
			//Disable assert reporting
			_CrtSetReportMode(_CRT_ASSERT, 0);

			//Set process dpi awareness
			SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);

			//Reset all used variables
			CaptureResetVariablesAll();

			//Update capture settings
			vCaptureSettings = captureSettings;

			//Initialize DirectX
			if (!InitializeDirectX()) { return false; }

			//Initialize sampler state
			if (!InitializeSamplerState()) { return false; }

			//Initialize render target view
			if (!InitializeRenderTargetView()) { return false; }

			//Initialize view port
			if (!InitializeViewPort()) { return false; }

			//Initialize shaders
			if (!InitializeShaders()) { return false; }

			//Set shader variables
			if (!SetShaderVariables()) { return false; }

			//Return capture details
			captureDetails = vCaptureDetails;
			return true;
		}
		catch (...)
		{
			std::cout << "InitializeCapture failed: " << hResult << std::endl;
			return false;
		}
	}
}