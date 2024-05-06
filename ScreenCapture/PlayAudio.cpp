#pragma once
#include "CaptureVariables.h"

namespace
{
	VOID PlayAudio(WCHAR* audioFile)
	{
		//Check settings
		if (!vCaptureSettings.SoundEffect)
		{
			return;
		}

		//Set resources
		CComPtr<IGraphBuilder> iGraphBuilder;
		CComPtr<IMediaControl> iMediaControl;
		CComPtr<IMediaSeeking> iMediaSeeking;
		CComPtr<IMediaEvent> iMediaEvent;
		try
		{
			//Create graph builder
			HRESULT hResult = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_ALL, IID_IGraphBuilder, (LPVOID*)&iGraphBuilder);

			//Convert variables
			iGraphBuilder->QueryInterface(&iMediaControl);
			iGraphBuilder->QueryInterface(&iMediaSeeking);
			iGraphBuilder->QueryInterface(&iMediaEvent);

			//Load audio file
			hResult = iGraphBuilder->RenderFile(audioFile, NULL);
			if (FAILED(hResult))
			{
				AVDebugWriteLine("Render audio file failed: " << audioFile);
				return;
			}

			//Play audio file
			hResult = iMediaControl->Run();
			if (FAILED(hResult))
			{
				AVDebugWriteLine("Run audio file failed: " << audioFile);
				return;
			}

			//Get audio duration
			LONGLONG audioDuration;
			iMediaSeeking->GetDuration(&audioDuration);
			LONGLONG audioDurationMs = audioDuration / 10000;
			if (audioDurationMs < 2500)
			{
				audioDurationMs = 2500;
			}

			//Wait for completion
			long eventCode;
			iMediaEvent->WaitForCompletion(audioDurationMs, &eventCode);
		}
		catch (...)
		{
			AVDebugWriteLine("Failed to play audio file: " << audioFile);
		}
		//Release resources
		iGraphBuilder.Release();
		iMediaControl.Release();
		iMediaSeeking.Release();
		iMediaEvent.Release();
	}
}