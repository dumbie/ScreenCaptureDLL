#pragma once
#include "CaptureVariables.h"

namespace
{
	CaptureResult PlayAudio(const WCHAR* audioFile)
	{
		try
		{
			//Check settings
			if (!vCaptureSettings.SoundEffect)
			{
				return { .Status = CaptureStatus::Success };
			}

			//Set resources
			CComPtr<IGraphBuilder> iGraphBuilder;
			CComPtr<IMediaControl> iMediaControl;
			CComPtr<IMediaSeeking> iMediaSeeking;
			CComPtr<IMediaEvent> iMediaEvent;

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
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"Render audio file failed") };
			}

			//Play audio file
			hResult = iMediaControl->Run();
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"Run audio file failed") };
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

			//Return result
			return { .Status = CaptureStatus::Success };
		}
		catch (...)
		{
			//Return result
			return { .Status = CaptureStatus::Failed, .Message = SysAllocString(L"PlayAudio failed") };
		}
	}
}