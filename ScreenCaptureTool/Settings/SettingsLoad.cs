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
                //Sound
                checkbox_CaptureSoundEffect.IsChecked = SettingLoad(vConfiguration, "SoundEffect", typeof(bool));

                //Location
                textblock_CaptureLocation.Text = textblock_CaptureLocation.Tag + SettingLoad(vConfiguration, "CaptureLocation", typeof(string));

                //Capture
                combobox_ImageSaveFormat.SelectedIndex = SettingLoad(vConfiguration, "ImageSaveFormat", typeof(int));

                textblock_ImageSaveQuality.Text = textblock_ImageSaveQuality.Tag + SettingLoad(vConfiguration, "ImageSaveQuality", typeof(string)) + "%";
                slider_ImageSaveQuality.Value = SettingLoad(vConfiguration, "ImageSaveQuality", typeof(double));

                //Shortcuts
                Load_SetShortcut_Items();
                Load_SetShortcut_Keys();
            }
            catch (Exception ex)
            {
                Debug.WriteLine("Failed to load application settings: " + ex.Message);
            }
        }
    }
}