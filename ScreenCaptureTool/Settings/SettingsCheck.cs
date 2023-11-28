using System;
using System.Diagnostics;
using static ArnoldVinkCode.AVSettings;
using static ScreenCapture.AppVariables;

namespace ScreenCapture
{
    public partial class WindowMain
    {
        //Check - Application Settings
        public void Settings_Check()
        {
            try
            {
                //First Launch
                if (!SettingCheck(vConfiguration, "AppFirstLaunch")) { SettingSave(vConfiguration, "AppFirstLaunch", "True"); }

                //Socket
                if (!SettingCheck(vConfiguration, "ServerPort")) { SettingSave(vConfiguration, "ServerPort", "1040"); }

                //General
                if (!SettingCheck(vConfiguration, "CaptureSoundEffect")) { SettingSave(vConfiguration, "CaptureSoundEffect", "True"); }
                if (!SettingCheck(vConfiguration, "CaptureMonitorId")) { SettingSave(vConfiguration, "CaptureMonitorId", "1"); }
                if (!SettingCheck(vConfiguration, "CaptureDrawBorder")) { SettingSave(vConfiguration, "CaptureDrawBorder", "False"); }
                if (!SettingCheck(vConfiguration, "CaptureDrawMouseCursor")) { SettingSave(vConfiguration, "CaptureDrawMouseCursor", "True"); }
                if (!SettingCheck(vConfiguration, "CaptureLocation")) { SettingSave(vConfiguration, "CaptureLocation", "Captures"); }

                //Screenshot
                if (!SettingCheck(vConfiguration, "ScreenshotSaveFormat")) { SettingSave(vConfiguration, "ScreenshotSaveFormat", "0"); }
                if (!SettingCheck(vConfiguration, "ScreenshotSaveQuality")) { SettingSave(vConfiguration, "ScreenshotSaveQuality", "80"); }
                if (!SettingCheck(vConfiguration, "ScreenshotMaxPixelDimension")) { SettingSave(vConfiguration, "ScreenshotMaxPixelDimension", "4320"); }

                //Recording
                if (!SettingCheck(vConfiguration, "VideoSaveFormat")) { SettingSave(vConfiguration, "VideoSaveFormat", "1"); }
                if (!SettingCheck(vConfiguration, "VideoFrameRate")) { SettingSave(vConfiguration, "VideoFrameRate", "60"); }
                if (!SettingCheck(vConfiguration, "VideoSaveQuality")) { SettingSave(vConfiguration, "VideoSaveQuality", "60"); }
                if (!SettingCheck(vConfiguration, "VideoMaxPixelDimension")) { SettingSave(vConfiguration, "VideoMaxPixelDimension", "1440"); }
                if (!SettingCheck(vConfiguration, "AudioSaveFormat")) { SettingSave(vConfiguration, "AudioSaveFormat", "1"); }
                if (!SettingCheck(vConfiguration, "AudioChannels")) { SettingSave(vConfiguration, "AudioChannels", "2"); }
                if (!SettingCheck(vConfiguration, "AudioBitRate")) { SettingSave(vConfiguration, "AudioBitRate", "256"); }
                if (!SettingCheck(vConfiguration, "AudioBitDepth")) { SettingSave(vConfiguration, "AudioBitDepth", "16"); }
                if (!SettingCheck(vConfiguration, "AudioSampleRate")) { SettingSave(vConfiguration, "AudioSampleRate", "48000"); }

                //Shortcuts
                Check_SetShortcut_Keys();
            }
            catch (Exception ex)
            {
                Debug.WriteLine("Failed to check the application settings: " + ex.Message);
            }
        }
    }
}