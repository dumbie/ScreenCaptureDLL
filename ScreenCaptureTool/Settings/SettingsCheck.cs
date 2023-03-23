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

                //Sound
                if (!SettingCheck(vConfiguration, "SoundScreenshot")) { SettingSave(vConfiguration, "SoundScreenshot", "True"); }

                //Location
                if (!SettingCheck(vConfiguration, "ScreenshotLocation")) { SettingSave(vConfiguration, "ScreenshotLocation", "Screenshots"); }

                //Capture
                if (!SettingCheck(vConfiguration, "ImageSaveFormat")) { SettingSave(vConfiguration, "ImageSaveFormat", "0"); }
                if (!SettingCheck(vConfiguration, "ImageSaveQuality")) { SettingSave(vConfiguration, "ImageSaveQuality", "100"); }

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