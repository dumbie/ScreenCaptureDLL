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
		__declspec(dllexport) CaptureResult CaptureInitialize(CaptureSettings captureSettings, BOOL forceInitialize)
		{
			try
			{
				return CaptureInitializeCode(captureSettings, forceInitialize);
			}
			catch (...)
			{
				return { .Status = CaptureStatus::Failed };
			}
		}

		__declspec(dllexport) CaptureDetails CaptureGetDetails()
		{
			return vCaptureDetails;
		}

		__declspec(dllexport) CaptureSettings CaptureGetSettings()
		{
			return vCaptureSettings;
		}

		__declspec(dllexport) CaptureResult CaptureUpdateSettings(CaptureSettings captureSettings)
		{
			try
			{
				//Check if initialized
				if (!vCaptureInstance.vInstanceInitialized)
				{
					return { .Status = CaptureStatus::Failed, .Message = SysAllocString(L"Capture is not initialized") };
				}

				//Update capture settings
				vCaptureSettings = captureSettings;

				//Set shader variables
				return SetShaderVariables();
			}
			catch (...)
			{
				return { .Status = CaptureStatus::Failed };
			}
		}

		__declspec(dllexport) CaptureResult CaptureReset()
		{
			try
			{
				return ResetVariablesAll();
			}
			catch (...)
			{
				return { .Status = CaptureStatus::Failed };
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

				//Return screen bytes
				return vCaptureInstance.vScreenBytesCache.data();
			}
			catch (...)
			{
				return NULL;
			}
		}

		__declspec(dllexport) CaptureResult CaptureImage(const WCHAR* filePath, UINT imageQuality, ImageFormats imageFormat)
		{
			try
			{
				return CaptureImageCode(filePath, imageQuality, imageFormat);
			}
			catch (...)
			{
				return { .Status = CaptureStatus::Failed };
			}
		}

		__declspec(dllexport) CaptureResult CaptureVideoStart(const WCHAR* filePath, MediaSettings mediaSettings)
		{
			try
			{
				return CaptureVideoStartCode(filePath, mediaSettings);
			}
			catch (...)
			{
				return { .Status = CaptureStatus::Failed };
			}
		}

		__declspec(dllexport) CaptureResult CaptureVideoStop()
		{
			try
			{
				return CaptureVideoStopCode();
			}
			catch (...)
			{
				return { .Status = CaptureStatus::Failed };
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