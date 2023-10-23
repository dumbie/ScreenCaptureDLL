using System;
using System.Diagnostics;

namespace ScreenCapture
{
    public partial class AppClose
    {
        //Exit application
        public static void Application_Exit()
        {
            try
            {
                Debug.WriteLine("Exiting application.");

                //Stop video capture
                ScreenCaptureCode.StopCaptureVideoToFile(false);

                //Hide the visible tray icon
                AppTrayMenu.TrayNotifyIcon.Visible = false;

                //Close the application
                Environment.Exit(0);
            }
            catch { }
        }
    }
}