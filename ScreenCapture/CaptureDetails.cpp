#pragma once
#include "CaptureVariables.h"

namespace
{
	BOOL GetMonitorPathInfo(HMONITOR hMonitor, DISPLAYCONFIG_PATH_INFO* pathInfo)
	{
		try
		{
			MONITORINFOEXW monitorInfo;
			monitorInfo.cbSize = sizeof(monitorInfo);
			if (!GetMonitorInfoW(hMonitor, &monitorInfo))
			{
				return false;
			}

			UINT displayPathCount;
			UINT displayModeCount;
			hResult = GetDisplayConfigBufferSizes(QDC_ONLY_ACTIVE_PATHS, &displayPathCount, &displayModeCount);
			if (FAILED(hResult))
			{
				return false;
			}

			std::vector<DISPLAYCONFIG_PATH_INFO> displayPathInfos(displayPathCount);
			std::vector<DISPLAYCONFIG_MODE_INFO> displayModeInfos(displayModeCount);
			hResult = QueryDisplayConfig(QDC_ONLY_ACTIVE_PATHS, &displayPathCount, displayPathInfos.data(), &displayModeCount, displayModeInfos.data(), NULL);
			if (FAILED(hResult))
			{
				return false;
			}

			for (DISPLAYCONFIG_PATH_INFO pathInfoLoop : displayPathInfos)
			{
				DISPLAYCONFIG_SOURCE_DEVICE_NAME sourceName{};
				sourceName.header.size = sizeof(sourceName);
				sourceName.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME;
				sourceName.header.adapterId = pathInfoLoop.targetInfo.adapterId;
				sourceName.header.id = pathInfoLoop.sourceInfo.id;
				hResult = DisplayConfigGetDeviceInfo(&sourceName.header);
				if (FAILED(hResult))
				{
					continue;
				}

				if (wcscmp(monitorInfo.szDevice, sourceName.viewGdiDeviceName) != 0)
				{
					continue;
				}

				*pathInfo = pathInfoLoop;
				return true;
			}
		}
		catch (...)
		{
			std::cout << "Failed to get monitor path info." << std::endl;
		}
		return false;
	}

	void GetMonitorResolution(HMONITOR hMonitor, UINT& pixelsWidth, UINT& pixelsHeight)
	{
		try
		{
			MONITORINFOEXA monitorInfo;
			monitorInfo.cbSize = sizeof(monitorInfo);
			if (!GetMonitorInfoA(hMonitor, &monitorInfo))
			{
				return;
			}

			DEVMODEA devMode;
			devMode.dmSize = sizeof(devMode);
			if (!EnumDisplaySettingsA(monitorInfo.szDevice, ENUM_CURRENT_SETTINGS, &devMode))
			{
				return;
			}

			pixelsWidth = devMode.dmPelsWidth;
			pixelsHeight = devMode.dmPelsHeight;
		}
		catch (...)
		{
			std::cout << "Failed to get monitor resolution." << std::endl;
		}
	}

	UINT GetMonitorRefreshRate(HMONITOR hMonitor)
	{
		try
		{
			MONITORINFOEXA monitorInfo;
			monitorInfo.cbSize = sizeof(monitorInfo);
			if (!GetMonitorInfoA(hMonitor, &monitorInfo))
			{
				return 0;
			}

			DEVMODEA devMode;
			devMode.dmSize = sizeof(devMode);
			if (!EnumDisplaySettingsA(monitorInfo.szDevice, ENUM_CURRENT_SETTINGS, &devMode))
			{
				return 0;
			}

			return devMode.dmDisplayFrequency;
		}
		catch (...)
		{
			std::cout << "Failed to get monitor refresh rate." << std::endl;
			return 0;
		}
	}

	FLOAT GetMonitorSDRWhiteLevel(HMONITOR hMonitor)
	{
		try
		{
			//Get display path info
			DISPLAYCONFIG_PATH_INFO pathInfo;
			if (!GetMonitorPathInfo(hMonitor, &pathInfo))
			{
				std::cout << "Failed to get monitor sdr white level." << std::endl;
				return 240.0F;
			}

			//Get display sdr white level
			DISPLAYCONFIG_SDR_WHITE_LEVEL sdrWhiteLevel{};
			sdrWhiteLevel.header.size = sizeof(sdrWhiteLevel);
			sdrWhiteLevel.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_SDR_WHITE_LEVEL;
			sdrWhiteLevel.header.adapterId = pathInfo.targetInfo.adapterId;
			sdrWhiteLevel.header.id = pathInfo.targetInfo.id;
			hResult = DisplayConfigGetDeviceInfo(&sdrWhiteLevel.header);
			if (FAILED(hResult))
			{
				std::cout << "Failed to get monitor sdr white level." << std::endl;
				return 240.0F;
			}
			else
			{
				return (sdrWhiteLevel.SDRWhiteLevel / 1000.0F) * 80.0F;
			}
		}
		catch (...)
		{
			std::cout << "Failed to get monitor sdr white level." << std::endl;
			return 240.0F;
		}
	}

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
					vCaptureInstance.iWicPixelFormatGuidSource = GUID_WICPixelFormat32bppBGRA;
					vCaptureInstance.vCaptureDxgiFormat = DXGI_FORMAT_B8G8R8A8_UNORM; //DXGI_FORMAT_B8G8R8A8_UNORM_SRGB
					vCaptureDetails.PixelByteSize = 4;
					vCaptureDetails.SDRWhiteLevel = GetMonitorSDRWhiteLevel(vDirectXInstance.iDxgiOutputDescription1.Monitor);
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
};