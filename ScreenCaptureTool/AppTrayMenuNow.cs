using ArnoldVinkCode;
using System;
using System.Diagnostics;
using System.Drawing;
using System.Reflection;
using System.Windows.Forms;
using static ScreenCapture.AppVariables;

namespace ScreenCapture
{
    public partial class AppTrayMenuNow
    {
        //Tray Menu Variables
        public static NotifyIcon TrayNotifyIcon = new NotifyIcon();
        public static ContextMenu TrayContextMenu = new ContextMenu();

        //Create the application tray menu
        public static void Application_CreateTrayMenu()
        {
            try
            {
                Debug.WriteLine("Creating application tray menu.");

                //Create a context menu for system tray
                TrayContextMenu.MenuItems.Add("Stop screen capture", NotifyIcon_Stop);

                //Initialize the tray notify icon
                TrayNotifyIcon.Text = AVFunctions.StringCut("Capturing " + vCaptureFileName, 59, "...");
                TrayNotifyIcon.Icon = new Icon(Assembly.GetEntryAssembly().GetManifestResourceStream("ScreenCaptureTool.Assets.AppIconRecording.ico"));

                //Add menu to tray icon and show it
                TrayNotifyIcon.ContextMenu = TrayContextMenu;
                TrayNotifyIcon.Visible = true;
            }
            catch { }
        }

        private static void NotifyIcon_Stop(object sender, EventArgs args)
        {
            try
            {
                AppClose.Application_Exit();
            }
            catch { }
        }
    }
}