#pragma once
#include "CaptureVariables.h"

namespace
{
	CaptureResult WriteMediaTexture2D(CComPtr<ID3D11Texture2D> mediaTexture, UINT mediaSize, BOOL mediaDiscontinuity, UINT mediaIndex, ULONGLONG mediaTimeStart, ULONGLONG mediaTimeDuration)
	{
		try
		{
			//Check media texture
			if (mediaTexture == NULL)
			{
				return { .Status = CaptureStatus::Failed, .Message = SysAllocString(L"Media texture is null") };
			}

			//Create media buffer
			CComPtr<IMFMediaBuffer> imfMediaBuffer;
			hResult = MFCreateDXGISurfaceBuffer(__uuidof(ID3D11Texture2D), mediaTexture, 0, false, &imfMediaBuffer);
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"Failed creating media buffer") };
			}

			//Set media buffer length
			if (mediaSize == 0)
			{
				DWORD imf2DBufferLength;
				CComPtr<IMF2DBuffer> imf2DBuffer;
				imfMediaBuffer->QueryInterface(__uuidof(IMF2DBuffer), (void**)&imf2DBuffer);
				imf2DBuffer->GetContiguousLength(&imf2DBufferLength);
				hResult = imfMediaBuffer->SetCurrentLength(imf2DBufferLength);
			}
			else
			{
				hResult = imfMediaBuffer->SetCurrentLength(mediaSize);
			}
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"Failed setting media buffer length") };
			}

			//Create media sample
			CComPtr<IMFSample> imfMediaSample;
			hResult = MFCreateSample(&imfMediaSample);
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"Failed creating media sample") };
			}

			//Set media sample discontinuity
			if (mediaDiscontinuity)
			{
				imfMediaSample->SetUINT32(MFSampleExtension_Discontinuity, 1);
			}

			//Add media buffer to sample
			hResult = imfMediaSample->AddBuffer(imfMediaBuffer);
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"Failed adding media sample buffer") };
			}

			//Set media start time
			hResult = imfMediaSample->SetSampleTime(mediaTimeStart);
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"Failed setting media start time") };
			}

			//Set media duration time
			hResult = imfMediaSample->SetSampleDuration(mediaTimeDuration);
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"Failed setting sample duration") };
			}

			//Write media to sample
			hResult = vMediaFoundationInstance.imfSinkWriter->WriteSample(mediaIndex, imfMediaSample);
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"Failed writing media sample") };
			}

			//Console debug output
			//AVDebugWriteLine("Written media texture sample: " << (mediaTimeStart / vReferenceTimeToSeconds) << "s/" << mediaTimeStart << " duration: " << mediaTimeDuration << " index: " << mediaIndex);

			//Return result
			return { .Status = CaptureStatus::Success };
		}
		catch (...)
		{
			//Return result
			return { .Status = CaptureStatus::Failed, .Message = SysAllocString(L"WriteMediaTexture2D failed") };
		}
	}

	CaptureResult WriteMediaDataBytes(std::vector<BYTE> mediaBytes, BOOL releaseBytes, BOOL mediaDiscontinuity, UINT mediaIndex, ULONGLONG mediaTimeStart, ULONGLONG mediaTimeDuration)
	{
		try
		{
			//Check media bytes
			if (mediaBytes.empty())
			{
				return { .Status = CaptureStatus::Failed, .Message = SysAllocString(L"Media bytes are null") };
			}

			//Create media buffer
			CComPtr<IMFMediaBuffer> imfMediaBuffer;
			hResult = MFCreateMemoryBuffer(mediaBytes.size(), &imfMediaBuffer);
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"Failed creating media buffer") };
			}

			//Lock media bytes
			BYTE* mediaBufferBytes;
			hResult = imfMediaBuffer->Lock(&mediaBufferBytes, NULL, NULL);
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"Failed locking media buffer") };
			}

			//Set media bytes
			memcpy(mediaBufferBytes, mediaBytes.data(), mediaBytes.size());

			//Unlock media bytes
			hResult = imfMediaBuffer->Unlock();
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"Failed unlocking media buffer") };
			}

			//Set media buffer length
			hResult = imfMediaBuffer->SetCurrentLength(mediaBytes.size());
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"Failed setting media buffer length") };
			}

			//Create media sample
			CComPtr<IMFSample> imfMediaSample;
			hResult = MFCreateSample(&imfMediaSample);
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"Failed creating media sample") };
			}

			//Set media sample discontinuity
			if (mediaDiscontinuity)
			{
				imfMediaSample->SetUINT32(MFSampleExtension_Discontinuity, 1);
			}

			//Add media buffer to sample
			hResult = imfMediaSample->AddBuffer(imfMediaBuffer);
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"Failed adding media sample buffer") };
			}

			//Set media start time
			hResult = imfMediaSample->SetSampleTime(mediaTimeStart);
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"Failed setting media start time") };
			}

			//Set media duration time
			hResult = imfMediaSample->SetSampleDuration(mediaTimeDuration);
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"Failed setting sample duration") };
			}

			//Write media to sample
			hResult = vMediaFoundationInstance.imfSinkWriter->WriteSample(mediaIndex, imfMediaSample);
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"Failed writing media sample") };
			}

			//Console debug output
			//AVDebugWriteLine("Written media bytes sample: " << (mediaTimeStart / vReferenceTimeToSeconds) << "s/" << mediaTimeStart << " duration: " << mediaTimeDuration << " size: " << mediaBytes.Size << " index: " << mediaIndex);

			//Clear media bytes
			if (releaseBytes)
			{
				mediaBytes.clear();
			}

			//Return result
			return { .Status = CaptureStatus::Success };
		}
		catch (...)
		{
			//Return result
			return { .Status = CaptureStatus::Failed, .Message = SysAllocString(L"WriteMediaDataBytes failed") };
		}
	}
}