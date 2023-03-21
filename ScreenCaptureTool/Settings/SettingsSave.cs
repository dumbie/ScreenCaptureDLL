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
                checkbox_ShortcutScreenshot.Click += (sender, e) =>
                {
                    SettingSave(vConfiguration, "ShortcutScreenshot", checkbox_ShortcutScreenshot.IsChecked);
                };

                //Sound
                checkbox_SoundScreenshot.Click += (sender, e) =>
                {
                    SettingSave(vConfiguration, "SoundScreenshot", checkbox_SoundScreenshot.IsChecked);
                };

                //Capture
                combobox_ImageSaveFormat.SelectionChanged += (sender, e) =>
                {
                    SettingSave(vConfiguration, "ImageSaveFormat", combobox_ImageSaveFormat.SelectedIndex);
                };

                slider_ImageSaveQuality.ValueChanged += (sender, e) =>
                {
                    textblock_ImageSaveQuality.Text = textblock_ImageSaveQuality.Tag + slider_ImageSaveQuality.Value.ToString() + "%";
                    SettingSave(vConfiguration, "ImageSaveQuality", slider_ImageSaveQuality.Value);
                };
            }
            catch (Exception ex)
            {
                Debug.WriteLine("Failed to save the application settings: " + ex.Message);
            }
        }
    }
}