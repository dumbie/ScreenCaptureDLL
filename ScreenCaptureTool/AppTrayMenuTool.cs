using System;
using System.Diagnostics;
using System.Drawing;
using System.Reflection;
using System.Windows.Forms;
using static ScreenCapture.AppVariables;

namespace ScreenCapture
{
    public partial class AppTrayMenuTool
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
                TrayContextMenu.MenuItems.Add("Open capture location", NotifyIcon_OpenCapture);
                TrayContextMenu.MenuItems.Add("-");
                TrayContextMenu.MenuItems.Add("Settings", NotifyIcon_Settings);
                TrayContextMenu.MenuItems.Add("Website", NotifyIcon_Website);
                TrayContextMenu.MenuItems.Add("Exit", NotifyIcon_Exit);

                //Initialize the tray notify icon
                TrayNotifyIcon.Text = "Screen Capture Tool";
                TrayNotifyIcon.Icon = new Icon(Assembly.GetEntryAssembly().GetManifestResourceStream("ScreenCaptureTool.Assets.AppIcon.ico"));

                //Handle Double Click event
                TrayNotifyIcon.DoubleClick += NotifyIcon_DoubleClick;

                //Add menu to tray icon and show it
                TrayNotifyIcon.ContextMenu = TrayContextMenu;
                TrayNotifyIcon.Visible = true;
            }
            catch { }
        }

        //Change tray icon
        public static void Application_ChangeTrayIcon(string iconName)
        {
            try
            {
                if (string.IsNullOrWhiteSpace(iconName))
                {
                    iconName = "AppIcon";
                }

                TrayNotifyIcon.Icon = new Icon(Assembly.GetEntryAssembly().GetManifestResourceStream("ScreenCaptureTool.Assets." + iconName + ".ico"));
            }
            catch { }
        }

        public static void NotifyIcon_DoubleClick(object sender, EventArgs args)
        {
            try
            {
                vWindowMain.Application_ShowHideWindow();
            }
            catch { }
        }

        public static void NotifyIcon_OpenCapture(object sender, EventArgs args)
        {
            try
            {
                vWindowMain.OpenCaptureLocation();
            }
            catch { }
        }

        public static void NotifyIcon_Settings(object sender, EventArgs args)
        {
            try
            {
                vWindowMain.Application_ShowHideWindow();
            }
            catch { }
        }

        public static void NotifyIcon_Website(object sender, EventArgs args)
        {
            try
            {
                Process.Start("https://projects.arnoldvink.com");
            }
            catch { }
        }

        public static async void NotifyIcon_Exit(object sender, EventArgs args)
        {
            try
            {
                await AppClose.Application_Exit_Prompt();
            }
            catch { }
        }
    }
}