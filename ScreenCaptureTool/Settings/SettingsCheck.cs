using System;
using System.Diagnostics;
using static ArnoldVinkCode.AVSettings;
using static ScreenCapture.AppVariables;

namespace ScreenCapture
{
    public partial class WindowMain
    {
        //Check - Application Settings
        void Settings_Check()
        {
            try
            {
                //First Launch
                if (!SettingCheck(vConfiguration, "AppFirstLaunch")) { SettingSave(vConfiguration, "AppFirstLaunch", "True"); }

                //Socket
                if (!SettingCheck(vConfiguration, "ServerPort")) { SettingSave(vConfiguration, "ServerPort", "1040"); }

                //General
                if (!SettingCheck(vConfiguration, "CaptureSoundEffect")) { SettingSave(vConfiguration, "CaptureSoundEffect", "True"); }
                if (!SettingCheck(vConfiguration, "CaptureMonitorId")) { SettingSave(vConfiguration, "CaptureMonitorId", "0"); }
                if (!SettingCheck(vConfiguration, "CaptureMouseDrawCursor")) { SettingSave(vConfiguration, "CaptureMouseDrawCursor", "False"); }
                if (!SettingCheck(vConfiguration, "CaptureLocation")) { SettingSave(vConfiguration, "CaptureLocation", "Captures"); }

                //Screenshot
                if (!SettingCheck(vConfiguration, "ScreenshotSaveFormat")) { SettingSave(vConfiguration, "ScreenshotSaveFormat", "0"); }
                if (!SettingCheck(vConfiguration, "ScreenshotSaveQuality")) { SettingSave(vConfiguration, "ScreenshotSaveQuality", "80"); }
                if (!SettingCheck(vConfiguration, "ScreenshotMaxPixelDimension")) { SettingSave(vConfiguration, "ScreenshotMaxPixelDimension", "4320"); }                

                //Recording

                //Shortcuts
                Check_SetShortcut_Keys();
            }
            catch (Exception ex)
            {
                Debug.WriteLine("Failed to check the application settings: " + ex.Message);
            }
        }
    }
}