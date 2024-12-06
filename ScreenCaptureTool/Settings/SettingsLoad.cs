using ArnoldVinkCode;
using System;
using System.Diagnostics;
using System.Linq;
using static ArnoldVinkCode.AVClasses;
using static ArnoldVinkCode.AVSettings;
using static ScreenCapture.AppVariables;

namespace ScreenCapture
{
    partial class WindowMain
    {
        //Load - Application Settings
        public void Settings_Load()
        {
            try
            {
                //ComboBox
                Load_ComboBox_Items();

                //General
                checkbox_CaptureSoundEffect.IsChecked = SettingLoad(vConfigurationScreenCaptureTool, "CaptureSoundEffect", typeof(bool));
                checkbox_CaptureDrawBorder.IsChecked = SettingLoad(vConfigurationScreenCaptureTool, "CaptureDrawBorder", typeof(bool));
                checkbox_CaptureDrawMouseCursor.IsChecked = SettingLoad(vConfigurationScreenCaptureTool, "CaptureDrawMouseCursor", typeof(bool));

                textblock_CaptureMonitorId.Text = textblock_CaptureMonitorId.Tag + SettingLoad(vConfigurationScreenCaptureTool, "CaptureMonitorId", typeof(string));
                slider_CaptureMonitorId.Value = SettingLoad(vConfigurationScreenCaptureTool, "CaptureMonitorId", typeof(double));

                //Saving
                checkbox_SaveWindowTitle.IsChecked = SettingLoad(vConfigurationScreenCaptureTool, "SaveWindowTitle", typeof(bool));
                textblock_CaptureLocation.Text = textblock_CaptureLocation.Tag + SettingLoad(vConfigurationScreenCaptureTool, "CaptureLocation", typeof(string));

                //Screenshot
                combobox_ScreenshotSaveFormat.SelectedIndex = SettingLoad(vConfigurationScreenCaptureTool, "ScreenshotSaveFormat", typeof(int));

                textblock_ScreenshotSaveQuality.Text = textblock_ScreenshotSaveQuality.Tag + SettingLoad(vConfigurationScreenCaptureTool, "ScreenshotSaveQuality", typeof(string)) + "%";
                slider_ScreenshotSaveQuality.Value = SettingLoad(vConfigurationScreenCaptureTool, "ScreenshotSaveQuality", typeof(double));

                textblock_ScreenshotMaxPixelDimension.Text = textblock_ScreenshotMaxPixelDimension.Tag + SettingLoad(vConfigurationScreenCaptureTool, "ScreenshotMaxPixelDimension", typeof(string)) + "px";
                slider_ScreenshotMaxPixelDimension.Value = SettingLoad(vConfigurationScreenCaptureTool, "ScreenshotMaxPixelDimension", typeof(double));

                //Recording
                combobox_VideoSaveFormat.SelectedIndex = SettingLoad(vConfigurationScreenCaptureTool, "VideoSaveFormat", typeof(int));

                int VideoFrameRate = SettingLoad(vConfigurationScreenCaptureTool, "VideoFrameRate", typeof(int));
                combobox_VideoFrameRate.SelectedItem = combobox_VideoFrameRate.Items.Cast<ComboBoxItemValue>().FirstOrDefault(x => x.Value == VideoFrameRate.ToString());

                combobox_VideoRateControl.SelectedIndex = SettingLoad(vConfigurationScreenCaptureTool, "VideoRateControl", typeof(int));

                textblock_VideoBitRate.Text = textblock_VideoBitRate.Tag + SettingLoad(vConfigurationScreenCaptureTool, "VideoBitRate", typeof(string)) + " Kbps";
                slider_VideoBitRate.Value = SettingLoad(vConfigurationScreenCaptureTool, "VideoBitRate", typeof(double));

                textblock_VideoMaxPixelDimension.Text = textblock_VideoMaxPixelDimension.Tag + SettingLoad(vConfigurationScreenCaptureTool, "VideoMaxPixelDimension", typeof(string)) + "px";
                slider_VideoMaxPixelDimension.Value = SettingLoad(vConfigurationScreenCaptureTool, "VideoMaxPixelDimension", typeof(double));

                combobox_AudioSaveFormat.SelectedIndex = SettingLoad(vConfigurationScreenCaptureTool, "AudioSaveFormat", typeof(int));

                int AudioChannels = SettingLoad(vConfigurationScreenCaptureTool, "AudioChannels", typeof(int));
                combobox_AudioChannels.SelectedItem = combobox_AudioChannels.Items.Cast<ComboBoxItemValue>().FirstOrDefault(x => x.Value == AudioChannels.ToString());

                textblock_AudioBitRate.Text = textblock_AudioBitRate.Tag + SettingLoad(vConfigurationScreenCaptureTool, "AudioBitRate", typeof(string)) + " kbps";
                slider_AudioBitRate.Value = SettingLoad(vConfigurationScreenCaptureTool, "AudioBitRate", typeof(double));

                int AudioBitDepth = SettingLoad(vConfigurationScreenCaptureTool, "AudioBitDepth", typeof(int));
                combobox_AudioBitDepth.SelectedItem = combobox_AudioBitDepth.Items.Cast<ComboBoxItemValue>().FirstOrDefault(x => x.Value == AudioBitDepth.ToString());

                int AudioSampleRate = SettingLoad(vConfigurationScreenCaptureTool, "AudioSampleRate", typeof(int));
                combobox_AudioSampleRate.SelectedItem = combobox_AudioSampleRate.Items.Cast<ComboBoxItemValue>().FirstOrDefault(x => x.Value == AudioSampleRate.ToString());

                //Overlay
                checkbox_OverlayShowScreenshot.IsChecked = SettingLoad(vConfigurationScreenCaptureTool, "OverlayShowScreenshot", typeof(bool));
                checkbox_OverlayShowRecording.IsChecked = SettingLoad(vConfigurationScreenCaptureTool, "OverlayShowRecording", typeof(bool));

                string OverlayPosition = SettingLoad(vConfigurationScreenCaptureTool, "OverlayPosition", typeof(string));
                combobox_OverlayPosition.SelectedItem = combobox_OverlayPosition.Items.Cast<ComboBoxItemValue>().FirstOrDefault(x => x.Value == OverlayPosition.ToString());

                //Launch on Windows startup
                checkbox_WindowsStartup.IsChecked = AVSettings.StartupShortcutCheck();
            }
            catch (Exception ex)
            {
                Debug.WriteLine("Failed to load application settings: " + ex.Message);
            }
        }
    }
}