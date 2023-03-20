using ArnoldVinkCode;
using System.Diagnostics;
using static ArnoldVinkCode.AVSettings;
using static ScreenCapture.AppVariables;

namespace ScreenCapture
{
    public partial class WindowMain
    {
        //Run application startup code
        public void Startup()
        {
            try
            {
                //Initialize Settings
                Settings_Check();
                Settings_Load();
                Settings_Save();

                //Create the tray menu
                Application_CreateTrayMenu();

                //Check settings if window needs to be shown
                if (SettingLoad(vConfiguration, "AppFirstLaunch", typeof(bool)))
                {
                    Debug.WriteLine("First launch showing the window.");
                    Application_ShowHideWindow();
                }

                //Register keyboard hotkeys
                AVInputOutputHotKey.Start();
                AVInputOutputHotKey.EventHotKeyPressed += AppHotKey.EventHotKeyPressed;
            }
            catch { }
        }
    }
}
