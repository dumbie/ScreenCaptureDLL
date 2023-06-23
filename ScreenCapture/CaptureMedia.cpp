#pragma once
#include "CaptureVariables.h"
#include "CaptureScreen.cpp"
#include "CaptureVideo.cpp"
#include "CaptureAudio.cpp"

namespace
{
	BOOL InitializeDxgiDeviceManager(UINT captureInstanceId)
	{
		try
		{
			//Create DXGI device manager
			UINT resetToken;
			hResult = MFCreateDXGIDeviceManager(&resetToken, &vCaptureInstances[captureInstanceId].imfDXGIDeviceManager);
			if (FAILED(hResult))
			{
				return false;
			}

			//Reset DXGI device manager
			hResult = vCaptureInstances[captureInstanceId].imfDXGIDeviceManager->ResetDevice(vCaptureInstances[captureInstanceId].vDirectXInstance.iD3D11Device5, resetToken);
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

	BOOL InitializeMediaFoundation(UINT captureInstanceId, WCHAR* filePath)
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
			imfAttributes->SetUINT32(MF_READWRITE_ENABLE_HARDWARE_TRANSFORMS, 1);
			imfAttributes->SetUINT32(MF_SINK_WRITER_DISABLE_THROTTLING, 1);
			imfAttributes->SetUnknown(MF_SINK_WRITER_D3D_MANAGER, vCaptureInstances[captureInstanceId].imfDXGIDeviceManager);
			imfAttributes->SetUnknown(MF_SOURCE_READER_D3D_MANAGER, vCaptureInstances[captureInstanceId].imfDXGIDeviceManager);
			imfAttributes->SetGUID(MF_TRANSCODE_CONTAINERTYPE, MFTranscodeContainerType_MPEG4);

			//Create IMF sink writer
			CComPtr<IMFSinkWriter> imfSinkWriterNormal;
			hResult = MFCreateSinkWriterFromURL(filePath, NULL, imfAttributes, &imfSinkWriterNormal);
			if (FAILED(hResult))
			{
				return false;
			}

			//Convert variables
			hResult = imfSinkWriterNormal->QueryInterface(&vCaptureInstances[captureInstanceId].imfSinkWriter);
			if (FAILED(hResult))
			{
				return false;
			}

			//Set video media type
			if (!SetVideoMediaType(captureInstanceId)) { return false; }

			//Set audio device capture (reads buffer)
			if (!SetAudioDeviceCapture(captureInstanceId)) { return false; }

			//Set audio device render (fills buffer)
			if (!SetAudioDeviceRender(captureInstanceId)) { return false; }

			//Set audio media type
			if (!SetAudioMediaType(captureInstanceId)) { return false; }

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