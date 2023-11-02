using System;
using System.Diagnostics;

namespace ScreenCapture
{
    public partial class CaptureScreen
    {
        public static void CaptureImageToFile()
        {
            try
            {
                Debug.WriteLine("Starting image capture process.");
                Process startProcess = new Process();
                startProcess.StartInfo.FileName = "ScreenCaptureNow.exe";
                startProcess.StartInfo.Arguments = "-image";
                startProcess.Start();
            }
            catch (Exception ex)
            {
                Debug.WriteLine("Screen image capture process start failed: " + ex.Message);
            }
        }
    }
}