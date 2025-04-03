using ArnoldVinkCode;
using ScreenCaptureImport;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Threading.Tasks;

namespace ScreenCapture
{
    public class AppExit
    {
        //Application close prompt
        public static async Task Exit_Prompt()
        {
            try
            {
                List<string> messageAnswers = new List<string>();
                messageAnswers.Add("Close application");
                messageAnswers.Add("Cancel");

                string messageResult = await new AVMessageBox().Popup(null, "Do you really want to close Screen Capture Tool?", "You will no longer be able to take screenshots using the set shortcuts.", messageAnswers);
                if (messageResult == "Close application")
                {
                    await Exit();
                }
            }
            catch { }
        }

        //Close application
        public static async Task Exit()
        {
            try
            {
                //Check exit status
                if (AppVariables.vApplicationExiting)
                {
                    Debug.WriteLine("Application is already exiting.");
                    return;
                }
                else
                {
                    Debug.WriteLine("Exiting application.");
                    AppVariables.vApplicationExiting = true;
                }

                //Stop active video capture
                try
                {
                    CaptureResult captureResult = CaptureScreen.StopCaptureVideoToFile();
                    Debug.WriteLine("Capture stop result: " + captureResult.Status + " / " + captureResult.ResultCode + " / " + captureResult.Message);
                }
                catch { }

                //Reset screen capture resources
                try
                {
                    CaptureResult captureResult = CaptureImport.CaptureReset();
                    Debug.WriteLine("Capture reset result: " + captureResult.Status + " / " + captureResult.ResultCode + " / " + captureResult.Message);
                }
                catch { }

                //Stop keyboard hotkeys
                await AVInputOutputHotkeyHook.Stop();

                //Hide the visible tray icons
                AVActions.DispatcherInvoke(delegate
                {
                    AppTrayMenuTool.TrayNotifyIcon.Visible = false;
                    AppTrayMenuCapture.TrayNotifyIcon.Visible = false;
                });

                //Close the application
                Environment.Exit(0);
            }
            catch (Exception ex)
            {
                Debug.WriteLine("Failed to exit application: " + ex.Message);
            }
        }
    }
}