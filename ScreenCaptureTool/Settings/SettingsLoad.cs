using System;
using System.Diagnostics;
using static ArnoldVinkCode.AVSettings;
using static ScreenCapture.AppVariables;

namespace ScreenCapture
{
    partial class WindowMain
    {
        //Load - Application Settings
        void Settings_Load()
        {
            try
            {
                //Shortcuts
                cb_SettingsShortcutScreenshotKeyboard.IsChecked = SettingLoad(vConfiguration, "ShortcutScreenshotKeyboard", typeof(bool));

                //Location
                textblock_Settings_ScreenshotLocation.Text = textblock_Settings_ScreenshotLocation.Tag + SettingLoad(vConfiguration, "ScreenshotLocation", typeof(string));

                //Capture
                cb_Settings_ScreenshotHDRtoSDR.IsChecked = SettingLoad(vConfiguration, "ScreenshotHDRtoSDR", typeof(bool));
            }
            catch (Exception ex)
            {
                Debug.WriteLine("Failed to load application settings: " + ex.Message);
            }
        }
    }
}