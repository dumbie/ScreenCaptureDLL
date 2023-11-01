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
			vCaptureInstance.vMediaCapturing = false;
			vCaptureInstance.vMediaWriteLoopAllowed = false;

			//Wait for loop to finish
			while (!vCaptureInstance.vMediaWriteLoopFinishedScreen && !vCaptureInstance.vMediaWriteLoopFinishedAudio)
			{
				std::cout << "Waiting for media capture loop to stop..." << std::endl;
				Sleep(1000);
			}

			//Finalize media write
			hResult = vCaptureInstance.imfSinkWriter->Finalize();
			if (FAILED(hResult))
			{
				std::cout << "Failed to finalize media capture..." << std::endl;
			}

			//Shutdown media foundation
			MFShutdown();

			//Release resources
			CaptureResetVariablesTexturesLoop();
			CaptureResetVariablesMedia();

			//Play audio effect
			PlayAudio(L"Assets\\Capture\\CaptureVideoStop.mp3");

			std::cout << "Stopped media capture..." << std::endl;
			return true;
		}
		catch (...)
		{
			//Release resources
			CaptureResetVariablesTexturesLoop();
			CaptureResetVariablesMedia();

			return false;
		}
	}
}