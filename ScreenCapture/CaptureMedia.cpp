#pragma once
#include "CaptureVariables.h"
#include "CaptureScreen.cpp"
#include "CaptureVideo.cpp"
#include "CaptureAudio.cpp"

namespace
{
	BOOL WriteMediaDataBytes(SafeBytes mediaBytes, BOOL releaseBytes, BOOL mediaDiscontinuity, UINT mediaIndex, ULONGLONG mediaTimeStart, ULONGLONG mediaTimeDuration)
	{
		try
		{
			//Check media bytes
			if (mediaBytes.IsEmpty())
			{
				return false;
			}

			//Create media buffer
			CComPtr<IMFMediaBuffer> imfMediaBuffer;
			hResult = MFCreateMemoryBuffer(mediaBytes.Size, &imfMediaBuffer);
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
			memcpy(mediaBufferBytes, mediaBytes.Data, mediaBytes.Size);

			//Unlock media bytes
			hResult = imfMediaBuffer->Unlock();
			if (FAILED(hResult))
			{
				return false;
			}

			//Set media length
			hResult = imfMediaBuffer->SetCurrentLength(mediaBytes.Size);
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
			hResult = imfSinkWriter->WriteSample(mediaIndex, imfMediaSample);
			if (FAILED(hResult))
			{
				return false;
			}

			//Console debug output
			//std::cout << "Written media sample: " << (mediaTimeStart / vReferenceTimeToSeconds) << "s/" << mediaTimeStart << " duration: " << mediaTimeDuration << " size: " << mediaBytes.Size << " index: " << mediaIndex << std::endl;

			//Release media bytes
			if (releaseBytes)
			{
				mediaBytes.Release();
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

	BOOL InitializeDxgiDeviceManager()
	{
		try
		{
			//Create DXGI device manager
			UINT resetToken;
			hResult = MFCreateDXGIDeviceManager(&resetToken, &imfDXGIDeviceManager);
			if (FAILED(hResult))
			{
				return false;
			}

			//Reset DXGI device manager
			hResult = imfDXGIDeviceManager->ResetDevice(iD3D11Device5, resetToken);
			if (FAILED(hResult))
			{
				return false;
			}

			//Return result
			std::cout << "DXGI device manager initialized." << std::endl;
			return true;
		}
		catch (...)
		{
			std::cout << "InitializeDxgiDeviceManager failed." << std::endl;
			return false;
		}
	}

	BOOL InitializeMediaFoundation(WCHAR* filePath)
	{
		try
		{
			//Start media foundation
			hResult = MFStartup(MFSTARTUP_LITE);
			if (FAILED(hResult))
			{
				return false;
			}

			//Create IMF attributes
			CComPtr<IMFAttributes> imfAttributes;
			hResult = MFCreateAttributes(&imfAttributes, 0);
			if (FAILED(hResult))
			{
				return false;
			}

			//Set IMF attributes
			imfAttributes->SetUINT32(MF_LOW_LATENCY, 1);
			imfAttributes->SetUINT32(MF_READWRITE_ENABLE_HARDWARE_TRANSFORMS, 1);
			imfAttributes->SetUINT32(MF_SINK_WRITER_DISABLE_THROTTLING, 1);
			imfAttributes->SetUnknown(MF_SINK_WRITER_D3D_MANAGER, imfDXGIDeviceManager);
			imfAttributes->SetUnknown(MF_SOURCE_READER_D3D_MANAGER, imfDXGIDeviceManager);
			imfAttributes->SetGUID(MF_TRANSCODE_CONTAINERTYPE, MFTranscodeContainerType_MPEG4);

			//Create IMF sink writer
			CComPtr<IMFSinkWriter> imfSinkWriterNormal;
			hResult = MFCreateSinkWriterFromURL(filePath, NULL, imfAttributes, &imfSinkWriterNormal);
			if (FAILED(hResult))
			{
				return false;
			}

			//Convert variables
			hResult = imfSinkWriterNormal->QueryInterface(&imfSinkWriter);
			if (FAILED(hResult))
			{
				return false;
			}

			//Set video media type
			if (!SetVideoMediaType()) { return false; }

			//Set audio device capture (reads buffer)
			if (!SetAudioDeviceCapture()) { return false; }

			//Set audio device render (fills buffer)
			if (!SetAudioDeviceRender()) { return false; }

			//Set audio media type
			if (!SetAudioMediaType()) { return false; }

			//Return result
			std::cout << "Media foundation initialized." << std::endl;
			return true;
		}
		catch (...)
		{
			std::cout << "InitializeMediaFoundation failed." << std::endl;
			return false;
		}
	}
}