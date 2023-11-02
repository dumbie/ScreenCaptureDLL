using ArnoldVinkCode;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Threading.Tasks;
using static ArnoldVinkCode.AVProcess;

namespace ScreenCapture
{
    public partial class CaptureScreen
    {
        public static async Task CaptureVideoToFile()
        {
            try
            {
                //Check if capture process is running
                List<ProcessMulti> screenCaptureProcess = Get_ProcessesMultiByName("ScreenCaptureNow", true);
                bool screenCapturerRunning = screenCaptureProcess.Any(x => x.Argument.Contains("-video"));
                if (screenCapturerRunning)
                {
                    Debug.WriteLine("Video capture process is running, stopping video capture.");
                    await ArnoldVinkPipes.PipeClientSendString("ScreenCapturer", 100, "-videostop");
                }
                else
                {
                    Debug.WriteLine("Starting video capture process.");
                    Process startProcess = new Process();
                    startProcess.StartInfo.FileName = "ScreenCaptureNow.exe";
                    startProcess.StartInfo.Arguments = "-video";
                    startProcess.Start();
                }
            }
            catch (Exception ex)
            {
                Debug.WriteLine("Screen video capture process start failed: " + ex.Message);
            }
        }
    }
}