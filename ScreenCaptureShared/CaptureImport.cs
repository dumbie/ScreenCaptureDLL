using System;
using System.Runtime.InteropServices;
using System.Security;

namespace ScreenCapture
{
    [SuppressUnmanagedCodeSecurity]
    public class CaptureImport
    {
        [DllImport("Resources\\ScreenCapture.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool CaptureInitialize(int captureInstanceId, CaptureSettings captureSettings, out CaptureDetails captureDetails);

        [DllImport("Resources\\ScreenCapture.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool CaptureUpdateSettings(int captureInstanceId, CaptureSettings captureSettings);

        [DllImport("Resources\\ScreenCapture.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool CaptureReset(int captureInstanceId);

        [DllImport("Resources\\ScreenCapture.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr CaptureScreenBytes(int captureInstanceId);

        [DllImport("Resources\\ScreenCapture.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool CaptureImage(int captureInstanceId, [MarshalAs(UnmanagedType.LPWStr)] string filePath, int imageQuality, ImageFormats imageFormat);

        [DllImport("Resources\\ScreenCapture.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool CaptureVideoStart(int captureInstanceId, [MarshalAs(UnmanagedType.LPWStr)] string filePath, MediaSettings mediaSettings);

        [DllImport("Resources\\ScreenCapture.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool CaptureVideoStop(int captureInstanceId);

        [DllImport("Resources\\ScreenCapture.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool CaptureVideoIsRecording(int captureInstanceId);
    }
}