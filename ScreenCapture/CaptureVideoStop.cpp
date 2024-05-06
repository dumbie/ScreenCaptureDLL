#pragma once
#include "CaptureVariables.h"
#include "CaptureReset.cpp"
#include "PlayAudio.cpp"

namespace
{
	BOOL CaptureVideoStopCode()
	{
		try
		{
			//Stop media capture loop
			MediaFoundationLoopStop();

			//Finalize media write
			hResult = vMediaFoundationInstance.imfSinkWriter->Finalize();
			if (FAILED(hResult))
			{
				AVDebugWriteLine("Failed to finalize media capture...");
			}

			//Shutdown media foundation
			MFShutdown();

			//Release resources
			TextureResetVariablesLoop();
			MediaFoundationResetVariablesAll();

			//Play audio effect
			PlayAudio(L"Assets\\Capture\\CaptureVideoStop.mp3");

			//Trigger capture event
			if (vCaptureEventVideoCaptureStopped)
			{
				std::thread threadEvent(vCaptureEventVideoCaptureStopped);
				threadEvent.detach();
			}

			AVDebugWriteLine("Stopped media capture...");
			return true;
		}
		catch (...)
		{
			//Release resources
			TextureResetVariablesLoop();
			MediaFoundationResetVariablesAll();

			return false;
		}
	}
}