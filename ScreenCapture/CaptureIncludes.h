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

//General
#include <atlbase.h>
#include <shellscalingapi.h>
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