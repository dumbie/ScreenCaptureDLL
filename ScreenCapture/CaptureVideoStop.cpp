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
				std::cout << "Failed to finalize media capture..." << std::endl;
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

			std::cout << "Stopped media capture..." << std::endl;
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