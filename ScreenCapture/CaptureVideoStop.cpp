#pragma once
#include "CaptureVariables.h"
#include "CaptureReset.cpp"
#include "PlayAudio.cpp"

namespace
{
	CaptureResult CaptureVideoStopCode()
	{
		AVFinally(
			{
				//Release resources
				TextureResetVariablesLoop();
				MediaFoundationResetVariablesAll();

				//Trigger capture event
				if (vCaptureEventVideoCaptureStopped)
				{
					std::thread threadEvent(vCaptureEventVideoCaptureStopped);
					threadEvent.detach();
				}
			});
		try
		{
			//Stop media capture loop
			MediaFoundationLoopStop();

			//Flush media capture
			hResult = vMediaFoundationInstance.imfSinkWriter->Flush(vMediaFoundationInstance.vImfSinkWriterIndex);
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .Message = SysAllocString(L"Failed flushing media capture") };
			}

			//Finalize media capture
			hResult = vMediaFoundationInstance.imfSinkWriter->Finalize();
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .Message = SysAllocString(L"Failed finalizing media capture") };
			}

			//Stop media foundation
			hResult = MFShutdown();
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"Failed stopping media foundation") };
			}

			//Play audio effect
			PlayAudio(L"Assets\\Capture\\CaptureVideoStop.mp3");

			//Return result
			AVDebugWriteLine("Stopped media capture successfully.");
			return { .Status = CaptureStatus::Success };
		}
		catch (...)
		{
			//Return result
			return { .Status = CaptureStatus::Failed, .Message = SysAllocString(L"Failed stopping media capture") };
		}
	}
}