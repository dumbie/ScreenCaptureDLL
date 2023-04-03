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

                //Shortcuts
                button_SetShortcutCaptureImage_Unmap.Click += Button_SetShortcut_Unmap_Click;
                combobox_SetShortcutCaptureImage_Key0.SelectionChanged += Combobox_SetShortcut_SelectionChanged;
                combobox_SetShortcutCaptureImage_Key1.SelectionChanged += Combobox_SetShortcut_SelectionChanged;
                combobox_SetShortcutCaptureImage_Key2.SelectionChanged += Combobox_SetShortcut_SelectionChanged;
            }
            catch (Exception ex)
            {
                Debug.WriteLine("Failed to save the application settings: " + ex.Message);
            }
        }
    }
}