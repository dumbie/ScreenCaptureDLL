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
                checkbox_ShortcutScreenshot.IsChecked = SettingLoad(vConfiguration, "ShortcutScreenshot", typeof(bool));

                //Sound
                checkbox_SoundScreenshot.IsChecked = SettingLoad(vConfiguration, "SoundScreenshot", typeof(bool));

                //Location
                textblock_ScreenshotLocation.Text = textblock_ScreenshotLocation.Tag + SettingLoad(vConfiguration, "ScreenshotLocation", typeof(string));

                //Capture
                combobox_ImageSaveFormat.SelectedIndex = SettingLoad(vConfiguration, "ImageSaveFormat", typeof(int));

                textblock_ImageSaveQuality.Text = textblock_ImageSaveQuality.Tag + SettingLoad(vConfiguration, "ImageSaveQuality", typeof(string)) + "%";
                slider_ImageSaveQuality.Value = SettingLoad(vConfiguration, "ImageSaveQuality", typeof(double));
            }
            catch (Exception ex)
            {
                Debug.WriteLine("Failed to load application settings: " + ex.Message);
            }
        }
    }
}