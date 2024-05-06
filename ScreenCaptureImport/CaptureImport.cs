using System;
using System.Runtime.InteropServices;
using System.Security;

namespace ScreenCaptureImport
{
    [SuppressUnmanagedCodeSecurity]
    public class CaptureImport
    {
        [DllImport("Resources\\ScreenCapture.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool CaptureInitialize(CaptureSettings captureSettings, out CaptureDetails captureDetails, bool forceInitialize);

        [DllImport("Resources\\ScreenCapture.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool CaptureUpdateSettings(CaptureSettings captureSettings);

        [DllImport("Resources\\ScreenCapture.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool CaptureReset();

        [DllImport("Resources\\ScreenCapture.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr CaptureScreenBytes();

        [DllImport("Resources\\ScreenCapture.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool CaptureImage([MarshalAs(UnmanagedType.LPWStr)] string filePath, int imageQuality, ImageFormats imageFormat);

        [DllImport("Resources\\ScreenCapture.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool CaptureVideoStart([MarshalAs(UnmanagedType.LPWStr)] string filePath, MediaSettings mediaSettings);

        [DllImport("Resources\\ScreenCapture.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool CaptureVideoStop();

        [DllImport("Resources\\ScreenCapture.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool CaptureVideoIsRecording();

        [DllImport("Resources\\ScreenCapture.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void CaptureEventVideoCaptureStopped(CaptureEvent captureEvent);

        [DllImport("Resources\\ScreenCapture.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void CaptureEventDeviceChangeDetected(CaptureEvent captureEvent);

        public delegate void CaptureEvent();
    }
}