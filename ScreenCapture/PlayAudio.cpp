#pragma once
#include "CaptureIncludes.h"
#include "CaptureVariables.h"

namespace
{
	VOID PlayAudio(UINT captureInstanceId, WCHAR* audioFile)
	{
		//Check settings
		if (!vCaptureInstances[captureInstanceId].vCaptureSettings.SoundEffect)
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
				std::wcout << "Render audio file failed: " << audioFile << std::endl;
				return;
			}

			//Play audio file
			hResult = iMediaControl->Run();
			if (FAILED(hResult))
			{
				std::wcout << "Run audio file failed: " << audioFile << std::endl;
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
			std::wcout << L"Failed to play audio file: " << audioFile << std::endl;
		}
		//Release resources
		iGraphBuilder.Release();
		iMediaControl.Release();
		iMediaSeeking.Release();
		iMediaEvent.Release();
	}
}