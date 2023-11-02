using ArnoldVinkCode;
using System;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Reflection;
using System.Threading.Tasks;
using static ArnoldVinkCode.AVSettings;
using static ScreenCapture.AppVariables;

namespace ScreenCapture
{
    public partial class ScreenCaptureCode
    {
        public static void StopCaptureVideoToFile(bool exitApplication)
        {
            try
            {
                Debug.WriteLine("Stopping video capturing...");

                //Check video capture
                if (CaptureImport.CaptureVideoIsRecording())
                {
                    //Update the tray notify icon
                    AppTrayMenu.TrayNotifyIcon.Text = AVFunctions.StringCut("Processing " + vCaptureFileName, 59, "...");
                    AppTrayMenu.TrayNotifyIcon.Icon = new Icon(Assembly.GetEntryAssembly().GetManifestResourceStream("ScreenCaptureNow.Assets.AppIconProcessing.ico"));

                    //Request to stop video capture
                    bool captureResultStop = CaptureImport.CaptureVideoStop();
                    Debug.WriteLine("Stopped video capturing: " + captureResultStop);
                }
            }
            catch (Exception ex)
            {
                Debug.WriteLine("Failed stopping video capture: " + ex.Message);
            }
            finally
            {
                //Reset screen capture resources
                CaptureImport.CaptureReset();

                //Exit application
                if (exitApplication)
                {
                    AppClose.Application_Exit();
                }
            }
        }

        public static async Task StartCaptureVideoToFile()
        {
            try
            {
                //Check video capture
                if (CaptureImport.CaptureVideoIsRecording())
                {
                    return;
                }

                //Capture tool settings
                VideoFormats VideoSaveFormat = (VideoFormats)SettingLoad(vConfigurationTool, "VideoSaveFormat", typeof(int));
                int VideoSaveQuality = SettingLoad(vConfigurationTool, "VideoSaveQuality", typeof(int));
                int VideoFrameRate = SettingLoad(vConfigurationTool, "VideoFrameRate", typeof(int));
                int VideoMaxPixelDimension = SettingLoad(vConfigurationTool, "VideoMaxPixelDimension", typeof(int));

                AudioFormats AudioSaveFormat = (AudioFormats)SettingLoad(vConfigurationTool, "AudioSaveFormat", typeof(int));
                int AudioChannels = SettingLoad(vConfigurationTool, "AudioChannels", typeof(int));
                int AudioBitRate = SettingLoad(vConfigurationTool, "AudioBitRate", typeof(int));
                int AudioBitDepth = SettingLoad(vConfigurationTool, "AudioBitDepth", typeof(int));
                int AudioSampleRate = SettingLoad(vConfigurationTool, "AudioSampleRate", typeof(int));

                int CaptureMonitorId = SettingLoad(vConfigurationTool, "CaptureMonitorId", typeof(int)) - 1;
                bool CaptureSoundEffect = SettingLoad(vConfigurationTool, "CaptureSoundEffect", typeof(bool));
                bool CaptureMouseDrawCursor = SettingLoad(vConfigurationTool, "CaptureMouseDrawCursor", typeof(bool));

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
                if (!CaptureImport.CaptureInitialize(captureSettings, out CaptureDetails vCaptureDetails, false))
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
                fileSaveName = "Video " + AVFiles.FileNameReplaceInvalidChars(fileSaveName, "-");
                vCaptureFileName = fileSaveName;

                //Check screenshot location
                string fileSaveFolder = SettingLoad(vConfigurationTool, "CaptureLocation", typeof(string));
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
                string fileSavePath = fileSaveFolder + "\\" + fileSaveName + ".mp4";

                //Start video capture
                bool captureStarted = CaptureImport.CaptureVideoStart(fileSavePath, mediaSettings);

                //Play capture sound
                if (captureStarted)
                {
                    //Create application tray menu
                    AppTrayMenu.Application_CreateTrayMenu();

                    //Start pipes server
                    SocketServer.EnablePipesServer();

                    Debug.WriteLine("Started screen capturing video.");
                }
                else
                {
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