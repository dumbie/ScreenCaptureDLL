#pragma once
#include "CaptureVariables.h"

namespace
{
	BOOL CodecCheckVideoEncoder(GUID subType)
	{
		try
		{
			UINT32 numMFTActivate = 0;
			CComHeapPtr<IMFActivate*> pppMFTActivate;
			MFT_REGISTER_TYPE_INFO mftRegisterTypeInfo = { MFMediaType_Video, subType };
			hResult = MFTEnumEx(MFT_CATEGORY_VIDEO_ENCODER, MFT_ENUM_FLAG_HARDWARE, NULL, &mftRegisterTypeInfo, &pppMFTActivate, &numMFTActivate);
			pppMFTActivate.Free();
			if (SUCCEEDED(hResult) && numMFTActivate > 0)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		catch (...)
		{
			AVDebugWriteLine("CodecCheckVideoEncoder failed.");
			return false;
		}
	}
}