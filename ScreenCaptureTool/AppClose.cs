using ArnoldVinkCode;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Threading.Tasks;

namespace ScreenCapture
{
    public partial class WindowMain
    {
        //Application Close Handler
        protected async override void OnClosing(CancelEventArgs e)
        {
            try
            {
                e.Cancel = true;
                await Application_Exit_Prompt();
            }
            catch { }
        }

        //Application close prompt
        public async Task Application_Exit_Prompt()
        {
            try
            {
                List<string> messageAnswers = new List<string>();
                messageAnswers.Add("Exit application");
                messageAnswers.Add("Cancel");

                string messageResult = await new AVMessageBox().Popup(this, "Do you really want to exit Screen Capture Tool?", "You will no longer be able to take screenshots using the set shortcuts.", messageAnswers);
                if (messageResult == "Exit application")
                {
                    Application_Exit();
                }
            }
            catch { }
        }

        //Close the application
        public void Application_Exit()
        {
            try
            {
                Debug.WriteLine("Exiting application.");

                //Hide the visible tray icon
                TrayNotifyIcon.Visible = false;

                //Close the application
                Environment.Exit(0);
            }
            catch { }
        }
    }
}