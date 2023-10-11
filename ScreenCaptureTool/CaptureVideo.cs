using ArnoldVinkCode;
using System;
using System.Diagnostics;
using System.IO;
using System.Threading.Tasks;
using static ArnoldVinkCode.AVSettings;
using static ScreenCapture.AppVariables;

namespace ScreenCapture
{
    public partial class CaptureScreen
    {
        //Prepare video capture
        public static async Task CaptureVideoToFile()
        {
            try
            {
                //Stop video capture
                if (CaptureImport.CaptureVideoIsRecording(1))
                {
                    Debug.WriteLine("Stopping video capturing...");

                    //Request to stop video capture
                    bool captureResultStop = CaptureImport.CaptureVideoStop(1);
                    Debug.WriteLine("Stopped video capturing: " + captureResultStop);

                    //Reset screen capture resources
                    CaptureImport.CaptureReset(1);

                    //Change tray icon
                    WindowMain.Application_ChangeTrayIcon("AppIcon");

                    return;
                }

                //Capture tool settings
                VideoFormats VideoSaveFormat = (VideoFormats)SettingLoad(vConfiguration, "VideoSaveFormat", typeof(int));
                int VideoSaveQuality = SettingLoad(vConfiguration, "VideoSaveQuality", typeof(int));
                int VideoFrameRate = SettingLoad(vConfiguration, "VideoFrameRate", typeof(int));
                int VideoMaxPixelDimension = SettingLoad(vConfiguration, "VideoMaxPixelDimension", typeof(int));

                AudioFormats AudioSaveFormat = (AudioFormats)SettingLoad(vConfiguration, "AudioSaveFormat", typeof(int));
                int AudioChannels = SettingLoad(vConfiguration, "AudioChannels", typeof(int));
                int AudioBitRate = SettingLoad(vConfiguration, "AudioBitRate", typeof(int));
                int AudioBitDepth = SettingLoad(vConfiguration, "AudioBitDepth", typeof(int));
                int AudioSampleRate = SettingLoad(vConfiguration, "AudioSampleRate", typeof(int));

                int CaptureMonitorId = SettingLoad(vConfiguration, "CaptureMonitorId", typeof(int)) - 1;
                bool CaptureSoundEffect = SettingLoad(vConfiguration, "CaptureSoundEffect", typeof(bool));
                bool CaptureMouseDrawCursor = SettingLoad(vConfiguration, "CaptureMouseDrawCursor", typeof(bool));

                //Screen capture settings
                CaptureSettings captureSettings = new CaptureSettings();
                captureSettings.HDRtoSDR = true;
                captureSettings.MaxPixelDimension = VideoMaxPixelDimension;
                captureSettings.MonitorId = CaptureMonitorId;
                captureSettings.SoundEffect = CaptureSoundEffect;
                captureSettings.MouseDrawCursor = CaptureMouseDrawCursor;

                //Media recording settings
                MediaSettings mediaSettings = new MediaSettings();
                mediaSettings.VideoFormat = VideoSaveFormat;
                mediaSettings.VideoFrameRate = VideoFrameRate;
                mediaSettings.VideoQuality = VideoSaveQuality;
                mediaSettings.AudioFormat = AudioSaveFormat;
                mediaSettings.AudioChannels = AudioChannels;
                mediaSettings.AudioBitRate = AudioBitRate;
                mediaSettings.AudioBitDepth = AudioBitDepth;
                mediaSettings.AudioSampleRate = AudioSampleRate;

                //Initialize screen capture
                if (!CaptureImport.CaptureInitialize(1, captureSettings, out CaptureDetails vCaptureDetails, false))
                {
                    Debug.WriteLine("Failed to initialize screen capture.");
                    return;
                }
                else
                {
                    //Wait for capturer to have initialized
                    await Task.Delay(500);
                }

                //Set screenshot name
                string fileSaveName = "(" + DateTime.Now.ToShortDateString() + ") " + DateTime.Now.ToString("HH.mm.ss.ffff");
                if (vCaptureDetails.HDREnabled)
                {
                    if (vCaptureDetails.HDRtoSDR)
                    {
                        fileSaveName += " (HDRtoSDR)";
                    }
                    else
                    {
                        fileSaveName += " (HDR)";
                    }
                }
                else
                {
                    fileSaveName += " (SDR)";
                }
                fileSaveName = "\\Recording " + AVFiles.FileNameReplaceInvalidChars(fileSaveName, "-");

                //Check screenshot location
                string fileSaveFolder = SettingLoad(vConfiguration, "CaptureLocation", typeof(string));
                if (string.IsNullOrWhiteSpace(fileSaveFolder) || !Directory.Exists(fileSaveFolder))
                {
                    //Check captures folder in app directory
                    if (!Directory.Exists("Captures"))
                    {
                        Directory.CreateDirectory("Captures");
                    }

                    //Set save folder to captures in app directory
                    fileSaveFolder = "Captures";
                }

                //Combine save path
                string fileSavePath = fileSaveFolder + fileSaveName + ".mp4";

                //Start video capture
                bool captureStarted = CaptureImport.CaptureVideoStart(1, fileSavePath, mediaSettings);

                //Play capture sound
                if (captureStarted)
                {
                    //Change tray icon
                    WindowMain.Application_ChangeTrayIcon("AppIconRecording");

                    Debug.WriteLine("Started screen capturing video.");
                }
                else
                {
                    //Change tray icon
                    WindowMain.Application_ChangeTrayIcon("AppIcon");

                    Debug.WriteLine("Failed screen capturing video.");
                }
            }
            catch (Exception ex)
            {
                Debug.WriteLine("Screen video capture failed: " + ex.Message);
            }
        }
    }
}