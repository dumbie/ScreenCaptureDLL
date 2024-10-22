#pragma once
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "windowscodecs.lib")
#pragma comment(lib, "wmcodecdspuuid.lib")
#pragma comment(lib, "mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "shcore.lib")
#pragma comment(lib, "strmiids.lib")
#pragma comment(lib, "avrt.lib")

//Arnold Vink Code
#include "..\..\ArnoldVinkCode\CPP\AVDebug.h"
#include "..\..\ArnoldVinkCode\CPP\AVHighResDelay.h"

//General
#include <atlbase.h>
#include <shellscalingapi.h>
#include <winternl.h>
#include <functional>
#include <vector>
#include <memory>
#include <thread>
#include <iostream>

//DirectX
#include <directxmath.h>
#include <dxgi1_6.h>
#include <d3d11_4.h>
#include <d3dcompiler.h>
#include <d3dkmthk.h>

//Codecs
#include <wincodec.h>
#include <strmif.h>
#include <codecapi.h>
#include <wmcodecdsp.h>

//Video
#include <mfapi.h>
#include <mfidl.h>
#include <mfobjects.h>
#include <mfreadwrite.h>
#include <mfmediaengine.h>
#include <mftransform.h>

//Audio
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <dshow.h>
#include <avrt.h>

//Windows Graphics Capture
#include <Inspectable.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.Capture.h>
#include <Windows.Graphics.Capture.Interop.h>
#include <Windows.Graphics.DirectX.Direct3D11.interop.h>