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
                if (!SettingCheck(vConfigurationScreenCaptureTool, "AppFirstLaunch")) { SettingSave(vConfigurationScreenCaptureTool, "AppFirstLaunch", "True"); }

                //General
                if (!SettingCheck(vConfigurationScreenCaptureTool, "CaptureSoundEffect")) { SettingSave(vConfigurationScreenCaptureTool, "CaptureSoundEffect", "True"); }
                if (!SettingCheck(vConfigurationScreenCaptureTool, "CaptureMonitorId")) { SettingSave(vConfigurationScreenCaptureTool, "CaptureMonitorId", "1"); }
                if (!SettingCheck(vConfigurationScreenCaptureTool, "CaptureDrawBorder")) { SettingSave(vConfigurationScreenCaptureTool, "CaptureDrawBorder", "False"); }
                if (!SettingCheck(vConfigurationScreenCaptureTool, "CaptureDrawMouseCursor")) { SettingSave(vConfigurationScreenCaptureTool, "CaptureDrawMouseCursor", "True"); }

                //Saving
                if (!SettingCheck(vConfigurationScreenCaptureTool, "SaveWindowTitle")) { SettingSave(vConfigurationScreenCaptureTool, "SaveWindowTitle", "True"); }
                if (!SettingCheck(vConfigurationScreenCaptureTool, "CaptureLocation")) { SettingSave(vConfigurationScreenCaptureTool, "CaptureLocation", "Captures"); }

                //Screenshot
                if (!SettingCheck(vConfigurationScreenCaptureTool, "ScreenshotSaveFormat")) { SettingSave(vConfigurationScreenCaptureTool, "ScreenshotSaveFormat", "0"); }
                if (!SettingCheck(vConfigurationScreenCaptureTool, "ScreenshotSaveQuality")) { SettingSave(vConfigurationScreenCaptureTool, "ScreenshotSaveQuality", "80"); }
                if (!SettingCheck(vConfigurationScreenCaptureTool, "ScreenshotMaxPixelDimension")) { SettingSave(vConfigurationScreenCaptureTool, "ScreenshotMaxPixelDimension", "4320"); }

                //Recording
                if (!SettingCheck(vConfigurationScreenCaptureTool, "VideoSaveFormat")) { SettingSave(vConfigurationScreenCaptureTool, "VideoSaveFormat", "0"); }
                if (!SettingCheck(vConfigurationScreenCaptureTool, "VideoFrameRate")) { SettingSave(vConfigurationScreenCaptureTool, "VideoFrameRate", "60"); }
                if (!SettingCheck(vConfigurationScreenCaptureTool, "VideoRateControl")) { SettingSave(vConfigurationScreenCaptureTool, "VideoRateControl", "0"); }
                if (!SettingCheck(vConfigurationScreenCaptureTool, "VideoBitRate")) { SettingSave(vConfigurationScreenCaptureTool, "VideoBitRate", "35000"); }
                if (!SettingCheck(vConfigurationScreenCaptureTool, "VideoMaxPixelDimension")) { SettingSave(vConfigurationScreenCaptureTool, "VideoMaxPixelDimension", "1440"); }
                if (!SettingCheck(vConfigurationScreenCaptureTool, "AudioSaveFormat")) { SettingSave(vConfigurationScreenCaptureTool, "AudioSaveFormat", "1"); }
                if (!SettingCheck(vConfigurationScreenCaptureTool, "AudioChannels")) { SettingSave(vConfigurationScreenCaptureTool, "AudioChannels", "2"); }
                if (!SettingCheck(vConfigurationScreenCaptureTool, "AudioBitRate")) { SettingSave(vConfigurationScreenCaptureTool, "AudioBitRate", "192"); }
                if (!SettingCheck(vConfigurationScreenCaptureTool, "AudioBitDepth")) { SettingSave(vConfigurationScreenCaptureTool, "AudioBitDepth", "16"); }
                if (!SettingCheck(vConfigurationScreenCaptureTool, "AudioSampleRate")) { SettingSave(vConfigurationScreenCaptureTool, "AudioSampleRate", "48000"); }

                //Overlay
                if (!SettingCheck(vConfigurationScreenCaptureTool, "OverlayShowScreenshot")) { SettingSave(vConfigurationScreenCaptureTool, "OverlayShowScreenshot", "True"); }
                if (!SettingCheck(vConfigurationScreenCaptureTool, "OverlayShowRecording")) { SettingSave(vConfigurationScreenCaptureTool, "OverlayShowRecording", "True"); }
                if (!SettingCheck(vConfigurationScreenCaptureTool, "OverlayPosition")) { SettingSave(vConfigurationScreenCaptureTool, "OverlayPosition", "BottomCenter"); }
            }
            catch (Exception ex)
            {
                Debug.WriteLine("Failed to check the application settings: " + ex.Message);
            }
        }
    }
}