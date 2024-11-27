#pragma once
#include "CaptureVariables.h"
#include "CaptureMonitorInfo.cpp"

namespace
{
	BOOL SetCaptureDetails()
	{
		try
		{
			//Get and set HDR details
			vCaptureDetails.HDREnabled = vDirectXInstance.iDxgiOutputDescription1.ColorSpace == DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020 || vDirectXInstance.iDxgiOutputDescription1.ColorSpace == DXGI_COLOR_SPACE_RGB_STUDIO_G2084_NONE_P2020;
			if (vCaptureDetails.HDREnabled)
			{
				vCaptureDetails.HDRtoSDR = vCaptureSettings.HDRtoSDR;
				if (vCaptureDetails.HDRtoSDR)
				{
					vBitmapImageInstance.iWicPixelFormatGuidSource = GUID_WICPixelFormat32bppBGRA;
					vCaptureInstance.vCaptureDxgiFormat = DXGI_FORMAT_B8G8R8A8_UNORM; //DXGI_FORMAT_B8G8R8A8_UNORM_SRGB
					vCaptureDetails.PixelByteSize = 4;
					vCaptureDetails.SDRWhiteLevel = GetMonitorSDRWhiteLevel(vDirectXInstance.iDxgiOutputDescription1.Monitor);
				}
				else
				{
					vBitmapImageInstance.iWicPixelFormatGuidSource = GUID_WICPixelFormat64bppRGBAHalf;
					vCaptureInstance.vCaptureDxgiFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
					vCaptureDetails.PixelByteSize = 8;
				}
			}
			else
			{
				vCaptureDetails.HDRtoSDR = false;
				vBitmapImageInstance.iWicPixelFormatGuidSource = GUID_WICPixelFormat32bppBGRA;
				vCaptureInstance.vCaptureDxgiFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
				vCaptureDetails.PixelByteSize = 4;
			}

			//Get monitor details
			UINT monitorPixelsWidth = 0;
			UINT monitorPixelsHeight = 0;
			UINT monitorRefreshRate = GetMonitorRefreshRate(vDirectXInstance.iDxgiOutputDescription1.Monitor);
			GetMonitorResolution(vDirectXInstance.iDxgiOutputDescription1.Monitor, monitorPixelsWidth, monitorPixelsHeight);

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
				UINT maxDimension = vCaptureSettings.MaxPixelDimension;
				//Find nearest full pixel dimensions to keep original ratio
				while (resizedWidth != (UINT)resizedWidth || resizedHeight != (UINT)resizedHeight)
				{
					DOUBLE differenceRatio = (DOUBLE)minDimension / maxDimension;
					resizedWidth = vCaptureDetails.OriginalWidth / differenceRatio;
					resizedHeight = vCaptureDetails.OriginalHeight / differenceRatio;
					maxDimension++;
				}
				vCaptureDetails.OutputWidth = resizedWidth;
				vCaptureDetails.OutputHeight = resizedHeight;
			}
			else
			{
				vCaptureDetails.OutputWidth = vCaptureDetails.OriginalWidth;
				vCaptureDetails.OutputHeight = vCaptureDetails.OriginalHeight;
			}
			AVDebugWriteLine("Screen capture output, Width: " << vCaptureDetails.OutputWidth << " Height: " << vCaptureDetails.OutputHeight);
			vCaptureDetails.WidthByteSize = vCaptureDetails.OutputWidth * vCaptureDetails.PixelByteSize;
			vCaptureDetails.TotalByteSize = vCaptureDetails.OutputWidth * vCaptureDetails.OutputHeight * vCaptureDetails.PixelByteSize;
			vCaptureInstance.vCaptureTextureMipLevels = 1 + log2(max(vCaptureDetails.OutputWidth, vCaptureDetails.OutputHeight));

			return true;
		}
		catch (...)
		{
			AVDebugWriteLine("SetCaptureDetails failed: " << hResult);
			return false;
		}
	}
};