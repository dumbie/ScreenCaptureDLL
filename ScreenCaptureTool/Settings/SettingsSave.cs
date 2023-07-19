using ArnoldVinkCode;
using System;
using System.Diagnostics;
using System.Windows.Controls;
using static ArnoldVinkCode.AVClasses;
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
                //General
                checkbox_CaptureSoundEffect.Click += (sender, e) =>
                {
                    SettingSave(vConfiguration, "CaptureSoundEffect", checkbox_CaptureSoundEffect.IsChecked);
                };

                checkbox_CaptureMouseDrawCursor.Click += (sender, e) =>
                {
                    SettingSave(vConfiguration, "CaptureMouseDrawCursor", checkbox_CaptureMouseDrawCursor.IsChecked);
                };

                slider_CaptureMonitorId.ValueChanged += (sender, e) =>
                {
                    textblock_CaptureMonitorId.Text = textblock_CaptureMonitorId.Tag + slider_CaptureMonitorId.Value.ToString();
                    SettingSave(vConfiguration, "CaptureMonitorId", slider_CaptureMonitorId.Value);
                };

                //Screenshot
                combobox_ScreenshotSaveFormat.SelectionChanged += (sender, e) =>
                {
                    SettingSave(vConfiguration, "ScreenshotSaveFormat", combobox_ScreenshotSaveFormat.SelectedIndex);
                };

                slider_ScreenshotSaveQuality.ValueChanged += (sender, e) =>
                {
                    textblock_ScreenshotSaveQuality.Text = textblock_ScreenshotSaveQuality.Tag + slider_ScreenshotSaveQuality.Value.ToString() + "%";
                    SettingSave(vConfiguration, "ScreenshotSaveQuality", slider_ScreenshotSaveQuality.Value);
                };

                slider_ScreenshotMaxPixelDimension.ValueChanged += (sender, e) =>
                {
                    textblock_ScreenshotMaxPixelDimension.Text = textblock_ScreenshotMaxPixelDimension.Tag + slider_ScreenshotMaxPixelDimension.Value.ToString() + "px";
                    SettingSave(vConfiguration, "ScreenshotMaxPixelDimension", slider_ScreenshotMaxPixelDimension.Value);
                };

                //Recording
                combobox_VideoSaveFormat.SelectionChanged += (sender, e) =>
                {
                    SettingSave(vConfiguration, "VideoSaveFormat", combobox_VideoSaveFormat.SelectedIndex);
                };

                combobox_VideoFrameRate.SelectionChanged += (sender, e) =>
                {
                    ComboBoxItemValue saveValue = (ComboBoxItemValue)combobox_VideoFrameRate.SelectedItem;
                    SettingSave(vConfiguration, "VideoFrameRate", saveValue.Value);
                };

                slider_VideoSaveQuality.ValueChanged += (sender, e) =>
                {
                    textblock_VideoSaveQuality.Text = textblock_VideoSaveQuality.Tag + slider_VideoSaveQuality.Value.ToString() + "%";
                    SettingSave(vConfiguration, "VideoSaveQuality", slider_VideoSaveQuality.Value);
                };

                slider_VideoMaxPixelDimension.ValueChanged += (sender, e) =>
                {
                    textblock_VideoMaxPixelDimension.Text = textblock_VideoMaxPixelDimension.Tag + slider_VideoMaxPixelDimension.Value.ToString() + "px";
                    SettingSave(vConfiguration, "VideoMaxPixelDimension", slider_VideoMaxPixelDimension.Value);
                };

                combobox_AudioSaveFormat.SelectionChanged += (sender, e) =>
                {
                    SettingSave(vConfiguration, "AudioSaveFormat", combobox_AudioSaveFormat.SelectedIndex);
                };

                combobox_AudioChannels.SelectionChanged += (sender, e) =>
                {
                    ComboBoxItemValue saveValue = (ComboBoxItemValue)combobox_AudioChannels.SelectedItem;
                    SettingSave(vConfiguration, "AudioChannels", saveValue.Value);
                };

                slider_AudioBitRate.ValueChanged += (sender, e) =>
                {
                    textblock_AudioBitRate.Text = textblock_AudioBitRate.Tag + slider_AudioBitRate.Value.ToString() + " kbps";
                    SettingSave(vConfiguration, "AudioBitRate", slider_AudioBitRate.Value);
                };

                combobox_AudioBitDepth.SelectionChanged += (sender, e) =>
                {
                    ComboBoxItemValue saveValue = (ComboBoxItemValue)combobox_AudioBitDepth.SelectedItem;
                    SettingSave(vConfiguration, "AudioBitDepth", saveValue.Value);
                };

                combobox_AudioSampleRate.SelectionChanged += (sender, e) =>
                {
                    ComboBoxItemValue saveValue = (ComboBoxItemValue)combobox_AudioSampleRate.SelectedItem;
                    SettingSave(vConfiguration, "AudioSampleRate", saveValue.Value);
                };

                checkbox_WindowsStartup.Click += (sender, e) =>
                {
                    try
                    {
                        AVSettings.StartupShortcutManage(string.Empty, true);
                    }
                    catch { }
                };

                //Shortcuts
                button_SetShortcutCaptureImage_Unmap.Click += Button_SetShortcut_Unmap_Image_Click;
                combobox_SetShortcutCaptureImage_Key0.SelectionChanged += Combobox_SetShortcut_SelectionChanged;
                combobox_SetShortcutCaptureImage_Key1.SelectionChanged += Combobox_SetShortcut_SelectionChanged;
                combobox_SetShortcutCaptureImage_Key2.SelectionChanged += Combobox_SetShortcut_SelectionChanged;

                button_SetShortcutCaptureVideo_Unmap.Click += Button_SetShortcut_Unmap_Video_Click;
                combobox_SetShortcutCaptureVideo_Key0.SelectionChanged += Combobox_SetShortcut_SelectionChanged;
                combobox_SetShortcutCaptureVideo_Key1.SelectionChanged += Combobox_SetShortcut_SelectionChanged;
                combobox_SetShortcutCaptureVideo_Key2.SelectionChanged += Combobox_SetShortcut_SelectionChanged;
            }
            catch (Exception ex)
            {
                Debug.WriteLine("Failed to save the application settings: " + ex.Message);
            }
        }
    }
}