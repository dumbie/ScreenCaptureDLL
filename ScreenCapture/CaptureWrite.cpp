#pragma once
#include "CaptureVariables.h"

namespace
{
	BOOL WriteMediaTexture2D(CComPtr<ID3D11Texture2D> mediaTexture, UINT mediaSize, BOOL mediaDiscontinuity, UINT mediaIndex, ULONGLONG mediaTimeStart, ULONGLONG mediaTimeDuration)
	{
		try
		{
			//Check media texture
			if (mediaTexture == NULL)
			{
				return false;
			}

			//Create media buffer
			CComPtr<IMFMediaBuffer> imfMediaBuffer;
			hResult = MFCreateDXGISurfaceBuffer(__uuidof(ID3D11Texture2D), mediaTexture, 0, false, &imfMediaBuffer);
			if (FAILED(hResult))
			{
				return false;
			}

			//Set buffer length
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
				return false;
			}

			//Create media sample
			CComPtr<IMFSample> imfMediaSample;
			hResult = MFCreateSample(&imfMediaSample);
			if (FAILED(hResult))
			{
				return false;
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
				return false;
			}

			//Set media start time
			hResult = imfMediaSample->SetSampleTime(mediaTimeStart);
			if (FAILED(hResult))
			{
				return false;
			}

			//Set media duration time
			hResult = imfMediaSample->SetSampleDuration(mediaTimeDuration);
			if (FAILED(hResult))
			{
				return false;
			}

			//Write media to sample
			hResult = vMediaFoundationInstance.imfSinkWriter->WriteSample(mediaIndex, imfMediaSample);
			if (FAILED(hResult))
			{
				std::cout << "Write texture sample failed: " << hResult << std::endl;
				return false;
			}

			//Console debug output
			//std::cout << "Written media texture sample: " << (mediaTimeStart / vReferenceTimeToSeconds) << "s/" << mediaTimeStart << " duration: " << mediaTimeDuration << " index: " << mediaIndex << std::endl;

			//Release resources
			imfMediaSample.Release();
			imfMediaBuffer.Release();

			//Return result
			return true;
		}
		catch (...)
		{
			std::cout << "WriteMediaTexture2D failed." << std::endl;
			return false;
		}
	}

	BOOL WriteMediaDataBytes(std::vector<BYTE> mediaBytes, BOOL releaseBytes, BOOL mediaDiscontinuity, UINT mediaIndex, ULONGLONG mediaTimeStart, ULONGLONG mediaTimeDuration)
	{
		try
		{
			//Check media bytes
			if (mediaBytes.empty())
			{
				return false;
			}

			//Create media buffer
			CComPtr<IMFMediaBuffer> imfMediaBuffer;
			hResult = MFCreateMemoryBuffer(mediaBytes.size(), &imfMediaBuffer);
			if (FAILED(hResult))
			{
				return false;
			}

			//Lock media bytes
			BYTE* mediaBufferBytes;
			hResult = imfMediaBuffer->Lock(&mediaBufferBytes, NULL, NULL);
			if (FAILED(hResult))
			{
				return false;
			}

			//Set media bytes
			memcpy(mediaBufferBytes, mediaBytes.data(), mediaBytes.size());

			//Unlock media bytes
			hResult = imfMediaBuffer->Unlock();
			if (FAILED(hResult))
			{
				return false;
			}

			//Set media length
			hResult = imfMediaBuffer->SetCurrentLength(mediaBytes.size());
			if (FAILED(hResult))
			{
				return false;
			}

			//Create media sample
			CComPtr<IMFSample> imfMediaSample;
			hResult = MFCreateSample(&imfMediaSample);
			if (FAILED(hResult))
			{
				return false;
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
				return false;
			}

			//Set media start time
			hResult = imfMediaSample->SetSampleTime(mediaTimeStart);
			if (FAILED(hResult))
			{
				return false;
			}

			//Set media duration time
			hResult = imfMediaSample->SetSampleDuration(mediaTimeDuration);
			if (FAILED(hResult))
			{
				return false;
			}

			//Write media to sample
			hResult = vMediaFoundationInstance.imfSinkWriter->WriteSample(mediaIndex, imfMediaSample);
			if (FAILED(hResult))
			{
				std::cout << "Write bytes sample failed: " << hResult << std::endl;
				return false;
			}

			//Console debug output
			//std::cout << "Written media bytes sample: " << (mediaTimeStart / vReferenceTimeToSeconds) << "s/" << mediaTimeStart << " duration: " << mediaTimeDuration << " size: " << mediaBytes.Size << " index: " << mediaIndex << std::endl;

			//Clear media bytes
			if (releaseBytes)
			{
				mediaBytes.clear();
			}

			//Release resources
			imfMediaSample.Release();
			imfMediaBuffer.Release();

			//Return result
			return true;
		}
		catch (...)
		{
			std::cout << "WriteMediaDataBytes failed." << std::endl;
			return false;
		}
	}
}