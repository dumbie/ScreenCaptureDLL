#pragma once
#include "CaptureVariables.h"

namespace
{
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

	FLOAT GetMonitorSDRWhiteLevel(UINT monitorId)
	{
		try
		{
			UINT displayPathCount;
			UINT displayModeCount;
			hResult = GetDisplayConfigBufferSizes(QDC_ONLY_ACTIVE_PATHS, &displayPathCount, &displayModeCount);
			if (FAILED(hResult))
			{
				return 240.0F;
			}

			DISPLAYCONFIG_PATH_INFO* pathInfos = new DISPLAYCONFIG_PATH_INFO[displayPathCount];
			DISPLAYCONFIG_MODE_INFO* modeInfos = new DISPLAYCONFIG_MODE_INFO[displayModeCount];
			hResult = QueryDisplayConfig(QDC_ONLY_ACTIVE_PATHS, &displayPathCount, pathInfos, &displayModeCount, modeInfos, NULL);
			if (FAILED(hResult))
			{
				return 240.0F;
			}

			DISPLAYCONFIG_PATH_INFO pathInfo = pathInfos[monitorId];

			DISPLAYCONFIG_SDR_WHITE_LEVEL sdrWhiteLevel{};
			sdrWhiteLevel.header.size = sizeof(sdrWhiteLevel);
			sdrWhiteLevel.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_SDR_WHITE_LEVEL;
			sdrWhiteLevel.header.adapterId = pathInfo.targetInfo.adapterId;
			sdrWhiteLevel.header.id = pathInfo.targetInfo.id;

			hResult = DisplayConfigGetDeviceInfo(&sdrWhiteLevel.header);
			if (FAILED(hResult))
			{
				return 240.0F;
			}

			return (sdrWhiteLevel.SDRWhiteLevel / 1000.0F) * 80.0F;
		}
		catch (...)
		{
			std::cout << "Failed to get monitor sdr white level." << std::endl;
			return 240.0F;
		}
	}
};