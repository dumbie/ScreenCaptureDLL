#pragma once
#include "CaptureVariables.h"
#include "CaptureInitAudio.cpp"
#include "CaptureInitVideo.cpp"

namespace
{
	CaptureResult InitializeDxgiDeviceManager()
	{
		try
		{
			//Create DXGI device manager
			UINT resetToken;
			hResult = MFCreateDXGIDeviceManager(&resetToken, &vMediaFoundationInstance.imfDXGIDeviceManager);
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"Failed creating DXGI device manager") };
			}

			//Reset DXGI device manager
			hResult = vMediaFoundationInstance.imfDXGIDeviceManager->ResetDevice(vDirectXInstance.iD3D11Device5, resetToken);
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"Failed resetting DXGI device manager") };
			}

			//Return result
			AVDebugWriteLine("DXGI device manager initialized.");
			return { .Status = CaptureStatus::Success };
		}
		catch (...)
		{
			//Return result
			return { .Status = CaptureStatus::Failed, .Message = SysAllocString(L"InitializeDxgiDeviceManager failed") };
		}
	}

	CaptureResult InitializeMediaFoundation(const WCHAR* filePath)
	{
		try
		{
			//Start media foundation
			hResult = MFStartup(MFSTARTUP_LITE);
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"Failed starting media foundation") };
			}

			//Create IMF attributes
			CComPtr<IMFAttributes> imfAttributes;
			hResult = MFCreateAttributes(&imfAttributes, 0);
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"Failed creating imf attributes") };
			}

			//Set IMF attributes
			imfAttributes->SetUINT32(MF_READWRITE_ENABLE_HARDWARE_TRANSFORMS, 1);
			imfAttributes->SetUINT32(MF_SINK_WRITER_DISABLE_THROTTLING, 1);
			imfAttributes->SetUnknown(MF_SINK_WRITER_D3D_MANAGER, vMediaFoundationInstance.imfDXGIDeviceManager);
			imfAttributes->SetUnknown(MF_SOURCE_READER_D3D_MANAGER, vMediaFoundationInstance.imfDXGIDeviceManager);
			imfAttributes->SetGUID(MF_TRANSCODE_CONTAINERTYPE, MFTranscodeContainerType_MPEG4);

			//Create IMF sink writer
			CComPtr<IMFSinkWriter> imfSinkWriter;
			hResult = MFCreateSinkWriterFromURL(filePath, NULL, imfAttributes, &imfSinkWriter);
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"Failed creating sink writer") };
			}

			//Convert variables
			hResult = imfSinkWriter->QueryInterface(&vMediaFoundationInstance.imfSinkWriter);
			if (FAILED(hResult))
			{
				return { .Status = CaptureStatus::Failed, .ResultCode = hResult, .Message = SysAllocString(L"Failed converting imfSinkWriter") };
			}

			//Set video media type
			capResult = SetVideoMediaType();
			if (capResult.Status != CaptureStatus::Success)
			{
				return capResult;
			}

			//Set audio device capture (reads buffer)
			capResult = SetAudioDeviceCapture();
			if (capResult.Status != CaptureStatus::Success)
			{
				return capResult;
			}

			//Set audio device render (fills buffer)
			capResult = SetAudioDeviceRender();
			if (capResult.Status != CaptureStatus::Success)
			{
				return capResult;
			}

			//Set audio out media type
			capResult = SetAudioMediaTypeOut();
			if (capResult.Status != CaptureStatus::Success)
			{
				return capResult;
			}

			//Set audio in media type
			capResult = SetAudioMediaTypeIn();
			if (capResult.Status != CaptureStatus::Success)
			{
				return capResult;
			}

			//Return result
			AVDebugWriteLine("Media foundation initialized.");
			return { .Status = CaptureStatus::Success };
		}
		catch (...)
		{
			//Return result
			return { .Status = CaptureStatus::Failed, .Message = SysAllocString(L"InitializeMediaFoundation failed") };
		}
	}
}