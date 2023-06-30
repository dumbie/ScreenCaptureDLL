﻿using ArnoldVinkCode;
using System;
using System.Diagnostics;
using System.Linq;
using System.Windows.Controls;
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

                //Recording
                combobox_VideoSaveFormat.SelectedIndex = SettingLoad(vConfiguration, "VideoSaveFormat", typeof(int));

                int VideoFrameRate = SettingLoad(vConfiguration, "VideoFrameRate", typeof(int));
                ComboBoxItem VideoFrameRateItem = combobox_VideoFrameRate.Items.Cast<ComboBoxItem>().Where(x => x.Content.ToString() == VideoFrameRate.ToString()).FirstOrDefault();
                combobox_VideoFrameRate.SelectedItem = VideoFrameRateItem;

                textblock_VideoSaveQuality.Text = textblock_VideoSaveQuality.Tag + SettingLoad(vConfiguration, "VideoSaveQuality", typeof(string)) + "%";
                slider_VideoSaveQuality.Value = SettingLoad(vConfiguration, "VideoSaveQuality", typeof(double));

                textblock_VideoMaxPixelDimension.Text = textblock_VideoMaxPixelDimension.Tag + SettingLoad(vConfiguration, "VideoMaxPixelDimension", typeof(string)) + "px";
                slider_VideoMaxPixelDimension.Value = SettingLoad(vConfiguration, "VideoMaxPixelDimension", typeof(double));

                combobox_AudioSaveFormat.SelectedIndex = SettingLoad(vConfiguration, "AudioSaveFormat", typeof(int));

                int AudioChannels = SettingLoad(vConfiguration, "AudioChannels", typeof(int));
                ComboBoxItem AudioChannelsItem = combobox_AudioChannels.Items.Cast<ComboBoxItem>().Where(x => x.Content.ToString() == AudioChannels.ToString()).FirstOrDefault();
                combobox_AudioChannels.SelectedItem = AudioChannelsItem;

                textblock_AudioBitRate.Text = textblock_AudioBitRate.Tag + SettingLoad(vConfiguration, "AudioBitRate", typeof(string));
                slider_AudioBitRate.Value = SettingLoad(vConfiguration, "AudioBitRate", typeof(double));

                int AudioBitDepth = SettingLoad(vConfiguration, "AudioBitDepth", typeof(int));
                ComboBoxItem AudioBitDepthItem = combobox_AudioBitDepth.Items.Cast<ComboBoxItem>().Where(x => x.Content.ToString() == AudioBitDepth.ToString()).FirstOrDefault();
                combobox_AudioBitDepth.SelectedItem = AudioBitDepthItem;

                int AudioSampleRate = SettingLoad(vConfiguration, "AudioSampleRate", typeof(int));
                ComboBoxItem AudioSampleRateItem = combobox_AudioSampleRate.Items.Cast<ComboBoxItem>().Where(x => x.Content.ToString() == AudioSampleRate.ToString()).FirstOrDefault();
                combobox_AudioSampleRate.SelectedItem = AudioSampleRateItem;

                //Check launch on Windows startup
                checkbox_WindowsStartup.IsChecked = AVSettings.StartupShortcutCheck();

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