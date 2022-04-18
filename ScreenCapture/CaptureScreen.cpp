#include "CaptureVariables.h"
#include "CaptureBitmap.cpp"
#include "CaptureInitialize.cpp"
#include "CaptureTexture.cpp"

namespace
{
	extern "C"
	{
		__declspec(dllexport) BOOL CaptureInitialize(CaptureSettings captureSettings, CaptureDetails* captureDetails)
		{
			try
			{
				//Disable assert reporting
				_CrtSetReportMode(_CRT_ASSERT, 0);

				//Reset all used variables
				CaptureResetVariablesAll();

				//Update capture settings
				vCaptureSettings = captureSettings;

				//Initialize DirectX
				if (!InitializeDirectX()) { return false; }

				//Initialize render target view
				if (!InitializeRenderTargetView()) { return false; }

				//Initialize view port
				if (!InitializeViewPort()) { return false; }

				//Initialize shaders
				if (!InitializeShaders()) { return false; }

				//Set shader variables
				if (!SetShaderVariables()) { return false; }

				//Return capture details
				*captureDetails = vCaptureDetails;
				return true;
			}
			catch (...)
			{
				return false;
			}
		}

		__declspec(dllexport) BOOL CaptureUpdateSettings(CaptureSettings captureSettings)
		{
			try
			{
				//Update capture settings
				vCaptureSettings = captureSettings;

				//Set shader variables
				return SetShaderVariables();
			}
			catch (...)
			{
				return false;
			}
		}

		__declspec(dllexport) BOOL CaptureUpdateSDRWhiteLevel()
		{
			try
			{
				//Update capture details
				vCaptureDetails.SDRWhiteLevel = GetMonitorSDRWhiteLevel();

				//Set shader variables
				return SetShaderVariables();
			}
			catch (...)
			{
				return false;
			}
		}

		__declspec(dllexport) BOOL CaptureReset()
		{
			try
			{
				return CaptureResetVariablesAll();
			}
			catch (...)
			{
				return false;
			}
		}

		__declspec(dllexport) BOOL CaptureFreeMemory(BYTE* bitmapData)
		{
			try
			{
				delete[] bitmapData;
				return true;
			}
			catch (...)
			{
				return false;
			}
		}

		__declspec(dllexport) BOOL CaptureSaveFileBmp(BYTE* bitmapData, WCHAR* filePath)
		{
			try
			{
				if (vCaptureDetails.HDREnabled && !vCaptureSettings.HDRtoSDR) { return false; }
				return BitmapDataSaveFile(bitmapData, filePath, GUID_ContainerFormatBmp);
			}
			catch (...)
			{
				return false;
			}
		}

		__declspec(dllexport) BOOL CaptureSaveFileJpg(BYTE* bitmapData, WCHAR* filePath, UINT imageQualityPercentage)
		{
			try
			{
				if (vCaptureDetails.HDREnabled && !vCaptureSettings.HDRtoSDR) { return false; }
				vBitmapImageQuality = imageQualityPercentage;
				return BitmapDataSaveFile(bitmapData, filePath, GUID_ContainerFormatJpeg);
			}
			catch (...)
			{
				return false;
			}
		}

		__declspec(dllexport) BOOL CaptureSaveFilePng(BYTE* bitmapData, WCHAR* filePath)
		{
			try
			{
				if (vCaptureDetails.HDREnabled && !vCaptureSettings.HDRtoSDR) { return false; }
				return BitmapDataSaveFile(bitmapData, filePath, GUID_ContainerFormatPng);
			}
			catch (...)
			{
				return false;
			}
		}

		__declspec(dllexport) BOOL CaptureSaveFileJxr(BYTE* bitmapData, WCHAR* filePath)
		{
			try
			{
				return BitmapDataSaveFile(bitmapData, filePath, GUID_ContainerFormatWmp);
			}
			catch (...)
			{
				return false;
			}
		}

		__declspec(dllexport) BYTE* CaptureScreenshot()
		{
			try
			{
				//Update SDR white level
				ULONGLONG currentTicks = GetTickCount64();
				if (vCaptureSettings.HDRtoSDR && (currentTicks - vCaptureSDRWhiteLevelTicks) > 3000)
				{
					CaptureUpdateSDRWhiteLevel();
					vCaptureSDRWhiteLevelTicks = currentTicks;
				}

				//Get output duplication frame
				DXGI_OUTDUPL_FRAME_INFO iDxgiOutputDuplicationFrameInfo;
				hResult = iDxgiOutputDuplication0->AcquireNextFrame(INFINITE, &iDxgiOutputDuplicationFrameInfo, &iDxgiResource0);
				if (FAILED(hResult))
				{
					CaptureResetVariablesScreenshot();
					return NULL;
				}

				//Convert variables
				hResult = iDxgiResource0->QueryInterface(&iD3D11Texture2D1Capture);
				if (FAILED(hResult))
				{
					CaptureResetVariablesScreenshot();
					return NULL;
				}

				//Apply shaders to texture
				if (!ApplyShadersToTexture2D(iD3D11Texture2D1Capture))
				{
					CaptureResetVariablesScreenshot();
					return NULL;
				}

				//Convert to cpu read texture
				if (!ConvertTexture2DtoCpuRead(iD3D11Texture2D1RenderTargetView))
				{
					CaptureResetVariablesScreenshot();
					return NULL;
				}

				//Convert texture to bitmap data
				BYTE* bitmapData = ConvertTexture2DtoBitmapData(iD3D11Texture2D1CpuRead);

				//Release output duplication frame
				hResult = iDxgiOutputDuplication0->ReleaseFrame();
				if (FAILED(hResult))
				{
					CaptureResetVariablesScreenshot();
					return NULL;
				}

				//Release resources
				CaptureResetVariablesScreenshot();

				//Return bitmap data
				return bitmapData;
			}
			catch (...)
			{
				CaptureResetVariablesScreenshot();
				return NULL;
			}
		}
	}
};