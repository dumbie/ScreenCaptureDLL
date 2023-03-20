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

                //Shortcuts
                if (!SettingCheck(vConfiguration, "ShortcutScreenshotKeyboard")) { SettingSave(vConfiguration, "ShortcutScreenshotKeyboard", "True"); }

                //Location
                if (!SettingCheck(vConfiguration, "ScreenshotLocation")) { SettingSave(vConfiguration, "ScreenshotLocation", "Screenshots"); }

                //Capture
                if (!SettingCheck(vConfiguration, "ScreenshotHDRtoSDR")) { SettingSave(vConfiguration, "ScreenshotHDRtoSDR", "False"); }
            }
            catch (Exception ex)
            {
                Debug.WriteLine("Failed to check the application settings: " + ex.Message);
            }
        }
    }
}