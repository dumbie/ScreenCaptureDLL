using System;
using System.Diagnostics;
using static ArnoldVinkCode.AVSettings;
using static ScreenCapture.AppVariables;

namespace ScreenCapture
{
    partial class WindowMain
    {
        //Save - Application Settings
        void Settings_Save()
        {
            try
            {
                //Shortcuts
                cb_SettingsShortcutScreenshotKeyboard.Click += (sender, e) =>
                {
                    SettingSave(vConfiguration, "ShortcutScreenshotKeyboard", cb_SettingsShortcutScreenshotKeyboard.IsChecked.ToString());
                };

                //Capture
                cb_Settings_ScreenshotHDRtoSDR.Click += (sender, e) =>
                {
                    SettingSave(vConfiguration, "ScreenshotHDRtoSDR", cb_Settings_ScreenshotHDRtoSDR.IsChecked.ToString());
                };
            }
            catch (Exception ex)
            {
                Debug.WriteLine("Failed to save the application settings: " + ex.Message);
            }
        }
    }
}