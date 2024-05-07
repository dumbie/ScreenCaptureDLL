#pragma once
#include "CaptureVariables.h"
#include "CaptureInitialize.cpp"
#include "CaptureImage.cpp"
#include "CaptureVideoStart.cpp"
#include "CaptureVideoStop.cpp"

namespace
{
	extern "C"
	{
		__declspec(dllexport) CaptureStatus CaptureInitialize(CaptureSettings captureSettings, CaptureDetails& captureDetails, BOOL forceInitialize)
		{
			try
			{
				return CaptureInitializeCode(captureSettings, captureDetails, forceInitialize);
			}
			catch (...)
			{
				return CaptureStatus::Failed;
			}
		}

		__declspec(dllexport) BOOL CaptureUpdateSettings(CaptureSettings captureSettings)
		{
			try
			{
				//Update capture settings
				vCaptureSettings = captureSettings;

				//Set shader variables
				return SetShaderVariables();
			}
			catch (...)
			{
				return false;
			}
		}

		__declspec(dllexport) BOOL CaptureReset()
		{
			try
			{
				return ResetVariablesAll();
			}
			catch (...)
			{
				return false;
			}
		}

		__declspec(dllexport) BYTE* CaptureScreenBytes()
		{
			try
			{
				//Clear screen bytes cache
				vCaptureInstance.vScreenBytesCache.clear();

				//Update screen bytes cache
				vCaptureInstance.vScreenBytesCache = GetScreenBytes(false);

				//Return result
				return vCaptureInstance.vScreenBytesCache.data();
			}
			catch (...)
			{
				return NULL;
			}
		}

		__declspec(dllexport) BOOL CaptureImage(WCHAR* filePath, UINT imageQuality, ImageFormats imageFormat)
		{
			try
			{
				return CaptureImageCode(filePath, imageQuality, imageFormat);
			}
			catch (...)
			{
				return false;
			}
		}

		__declspec(dllexport) BOOL CaptureVideoStart(WCHAR* filePath, MediaSettings mediaSettings)
		{
			try
			{
				return CaptureVideoStartCode(filePath, mediaSettings);
			}
			catch (...)
			{
				return false;
			}
		}

		__declspec(dllexport) BOOL CaptureVideoStop()
		{
			try
			{
				return CaptureVideoStopCode();
			}
			catch (...)
			{
				return false;
			}
		}

		__declspec(dllexport) BOOL CaptureVideoIsRecording()
		{
			return vMediaFoundationInstance.vMediaCapturing;
		}

		__declspec(dllexport) void CaptureEventVideoCaptureStopped(CaptureEvent captureEvent)
		{
			vCaptureEventVideoCaptureStopped = captureEvent;
		}

		__declspec(dllexport) void CaptureEventDeviceChangeDetected(CaptureEvent captureEvent)
		{
			vCaptureEventDeviceChangeDetected = captureEvent;
		}
	}
};