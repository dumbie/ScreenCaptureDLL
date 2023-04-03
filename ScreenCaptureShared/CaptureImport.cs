using System;
using System.Runtime.InteropServices;

namespace ScreenCapture
{
    public class CaptureImport
    {
        [DllImport("Resources\\ScreenCapture.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool CaptureInitialize(CaptureSettings captureSettings, out CaptureDetails captureDetails);

        [DllImport("Resources\\ScreenCapture.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool CaptureUpdateSettings(CaptureSettings captureSettings);

        [DllImport("Resources\\ScreenCapture.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool CaptureUpdateSDRWhiteLevel();

        [DllImport("Resources\\ScreenCapture.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool CaptureReset();

        [DllImport("Resources\\ScreenCapture.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool CaptureFreeMemory(IntPtr bitmapData);

        [DllImport("Resources\\ScreenCapture.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr CaptureScreenshot();

        [DllImport("Resources\\ScreenCapture.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool CaptureImage(IntPtr bitmapData, [MarshalAs(UnmanagedType.LPWStr)] string filePath, int imageQualityPercentage, ImageFormats imageFormat);

        [DllImport("Resources\\ScreenCapture.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool CaptureVideoStartStop();

        [DllImport("Resources\\ScreenCapture.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool CaptureVideoStart();

        [DllImport("Resources\\ScreenCapture.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool CaptureVideoStop();
    }
}