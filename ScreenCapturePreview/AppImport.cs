using System;
using System.Runtime.InteropServices;

namespace ScreenCapturePreview
{
    class AppImport
    {
        [DllImport("Resources\\ScreenCapture.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool CaptureInitialize(int captureMonitorId, int maxPixelDimension, bool convertHDRtoSDR, out int captureWidth, out int captureHeight, out int captureWidthByteSize, out int captureTotalByteSize, out bool captureHDREnabled);

        [DllImport("Resources\\ScreenCapture.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool CaptureReset();

        [DllImport("Resources\\ScreenCapture.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool CaptureFreeMemory(IntPtr bitmapData);

        [DllImport("Resources\\ScreenCapture.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr CaptureScreenshot();

        [DllImport("Resources\\ScreenCapture.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool CaptureSaveFileBmp(IntPtr bitmapData, [MarshalAs(UnmanagedType.LPWStr)] string filePath);

        [DllImport("Resources\\ScreenCapture.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool CaptureSaveFileJpg(IntPtr bitmapData, [MarshalAs(UnmanagedType.LPWStr)] string filePath, int imageQualityPercentage);

        [DllImport("Resources\\ScreenCapture.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool CaptureSaveFilePng(IntPtr bitmapData, [MarshalAs(UnmanagedType.LPWStr)] string filePath);

        [DllImport("Resources\\ScreenCapture.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool CaptureSaveFileJxr(IntPtr bitmapData, [MarshalAs(UnmanagedType.LPWStr)] string filePath);
    }
}