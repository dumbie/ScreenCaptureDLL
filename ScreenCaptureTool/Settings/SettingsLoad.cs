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
                //General
                checkbox_CaptureSoundEffect.IsChecked = SettingLoad(vConfiguration, "CaptureSoundEffect", typeof(bool));
                checkbox_CaptureMouseDrawCursor.IsChecked = SettingLoad(vConfiguration, "CaptureMouseDrawCursor", typeof(bool));
                textblock_CaptureLocation.Text = textblock_CaptureLocation.Tag + SettingLoad(vConfiguration, "CaptureLocation", typeof(string));

                textblock_CaptureMonitorId.Text = textblock_CaptureMonitorId.Tag + SettingLoad(vConfiguration, "CaptureMonitorId", typeof(string));
                slider_CaptureMonitorId.Value = SettingLoad(vConfiguration, "CaptureMonitorId", typeof(double));

                //Screenshot
                combobox_ScreenshotSaveFormat.SelectedIndex = SettingLoad(vConfiguration, "ScreenshotSaveFormat", typeof(int));

                textblock_ScreenshotSaveQuality.Text = textblock_ScreenshotSaveQuality.Tag + SettingLoad(vConfiguration, "ScreenshotSaveQuality", typeof(string)) + "%";
                slider_ScreenshotSaveQuality.Value = SettingLoad(vConfiguration, "ScreenshotSaveQuality", typeof(double));

                textblock_ScreenshotMaxPixelDimension.Text = textblock_ScreenshotMaxPixelDimension.Tag + SettingLoad(vConfiguration, "ScreenshotMaxPixelDimension", typeof(string)) + "px";
                slider_ScreenshotMaxPixelDimension.Value = SettingLoad(vConfiguration, "ScreenshotMaxPixelDimension", typeof(double));

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