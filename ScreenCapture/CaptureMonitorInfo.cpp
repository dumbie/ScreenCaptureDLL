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
			AVDebugWriteLine("Failed to get monitor path info.");
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
			AVDebugWriteLine("Failed to get monitor resolution.");
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
			AVDebugWriteLine("Failed to get monitor refresh rate.");
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
				AVDebugWriteLine("Failed to get monitor sdr white level.");
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
				AVDebugWriteLine("Failed to get monitor sdr white level.");
				return 240.0F;
			}
			else
			{
				return (sdrWhiteLevel.SDRWhiteLevel / 1000.0F) * 80.0F;
			}
		}
		catch (...)
		{
			AVDebugWriteLine("Failed to get monitor sdr white level.");
			return 240.0F;
		}
	}

	BOOL GetMonitorIsLimitedColorRange()
	{
		try
		{
			//Fix find a way to get current ColorSpace (full or limited)
			//Fix iDxgiOutputDescription1 returning wrong ColorSpace
			//Fix convert enum to string and check for _STUDIO_
			//return vDirectXInstance.iDxgiOutputDescription1.ColorSpace == DXGI_COLOR_SPACE_RGB_STUDIO_G22_NONE_P709
			//	|| vDirectXInstance.iDxgiOutputDescription1.ColorSpace == DXGI_COLOR_SPACE_RGB_STUDIO_G22_NONE_P2020
			//	|| vDirectXInstance.iDxgiOutputDescription1.ColorSpace == DXGI_COLOR_SPACE_RGB_STUDIO_G2084_NONE_P2020
			//	|| vDirectXInstance.iDxgiOutputDescription1.ColorSpace == DXGI_COLOR_SPACE_RGB_STUDIO_G24_NONE_P709
			//	|| vDirectXInstance.iDxgiOutputDescription1.ColorSpace == DXGI_COLOR_SPACE_RGB_STUDIO_G24_NONE_P2020;
			return true;
		}
		catch (...)
		{
			AVDebugWriteLine("Failed to get monitor color range.");
			return true;
		}
	}
};