#pragma once
#include "CaptureVariables.h"
#include "CaptureInitAudio.cpp"
#include "CaptureInitVideo.cpp"

namespace
{
	BOOL InitializeDxgiDeviceManager()
	{
		try
		{
			//Create DXGI device manager
			UINT resetToken;
			hResult = MFCreateDXGIDeviceManager(&resetToken, &vMediaFoundationInstance.imfDXGIDeviceManager);
			if (FAILED(hResult))
			{
				return false;
			}

			//Reset DXGI device manager
			hResult = vMediaFoundationInstance.imfDXGIDeviceManager->ResetDevice(vDirectXInstance.iD3D11Device5, resetToken);
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
			imfAttributes->SetUINT32(MF_READWRITE_ENABLE_HARDWARE_TRANSFORMS, 1);
			imfAttributes->SetUINT32(MF_SINK_WRITER_DISABLE_THROTTLING, 1);
			imfAttributes->SetUnknown(MF_SINK_WRITER_D3D_MANAGER, vMediaFoundationInstance.imfDXGIDeviceManager);
			imfAttributes->SetUnknown(MF_SOURCE_READER_D3D_MANAGER, vMediaFoundationInstance.imfDXGIDeviceManager);
			imfAttributes->SetGUID(MF_TRANSCODE_CONTAINERTYPE, MFTranscodeContainerType_MPEG4);

			//Create IMF sink writer
			CComPtr<IMFSinkWriter> imfSinkWriterNormal;
			hResult = MFCreateSinkWriterFromURL(filePath, NULL, imfAttributes, &imfSinkWriterNormal);
			if (FAILED(hResult))
			{
				return false;
			}

			//Convert variables
			hResult = imfSinkWriterNormal->QueryInterface(&vMediaFoundationInstance.imfSinkWriter);
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