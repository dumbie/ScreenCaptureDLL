#pragma once
#include "CaptureIncludes.h"

namespace
{
	VOID PlayAudio(WCHAR* audioFile)
	{
		//Set resources
		CComPtr<IGraphBuilder> iGraphBuilder;
		CComPtr<IMediaControl> iMediaControl;
		CComPtr<IMediaSeeking> iMediaSeeking;
		try
		{
			//Create graph builder
			HRESULT hResult = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_ALL, IID_IGraphBuilder, (LPVOID*)&iGraphBuilder);

			//Convert variables
			iGraphBuilder->QueryInterface(&iMediaControl);
			iGraphBuilder->QueryInterface(&iMediaSeeking);

			//Load audio file
			iGraphBuilder->RenderFile(audioFile, NULL);

			//Play audio file
			iMediaControl->Run();

			//Wait for completion
			LONGLONG audioDuration;
			iMediaSeeking->GetDuration(&audioDuration);
			Sleep(audioDuration / 10000);
		}
		catch (...) {}
		//Release resources
		iGraphBuilder.Release();
		iMediaControl.Release();
		iMediaSeeking.Release();
	}
}