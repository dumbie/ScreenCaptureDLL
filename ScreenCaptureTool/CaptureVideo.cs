﻿using ArnoldVinkCode;
using ScreenCaptureImport;
using System;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Media;
using static ArnoldVinkCode.AVInteropDll;
using static ArnoldVinkCode.AVProcess;
using static ArnoldVinkCode.AVSettings;
using static ScreenCapture.AppVariables;

namespace ScreenCapture
{
    public partial class CaptureScreen
    {
        public static async Task CaptureVideoProcess(int captureDelay)
        {
            try
            {
                //Capture start stop delay
                await Task.Delay(captureDelay);

                //Check active video capture
                if (CaptureImport.CaptureVideoIsRecording())
                {
                    Debug.WriteLine("Video capture process is running, stopping video capture.");
                    await AppExit.Exit();
                }
                else
                {
                    Debug.WriteLine("Starting video capture process.");
                    Process startProcess = new Process();
                    startProcess.StartInfo.FileName = "ScreenCaptureTool.exe";
                    startProcess.StartInfo.Arguments = "-video";
                    startProcess.Start();
                }
            }
            catch (Exception ex)
            {
                Debug.WriteLine("Screen video capture process start failed: " + ex.Message);
            }
        }

        public static CaptureResult StopCaptureVideoToFile()
        {
            try
            {
                //Check video capture
                if (CaptureImport.CaptureVideoIsRecording())
                {
                    Debug.WriteLine("Stopping active video capture...");

                    //Update the tray notify icon
                    AppTrayMenuCapture.TrayNotifyIcon.Text = AVFunctions.StringCut("Processing " + vCaptureFileName, 59, "...");
                    AppTrayMenuCapture.TrayNotifyIcon.Icon = new Icon(AVEmbedded.EmbeddedResourceToStream(null, "ScreenCaptureTool.Assets.AppIconProcessing.ico"));
                    AVActions.DispatcherInvoke(delegate
                    {
                        vWindowOverlay.ellipse_Status.Fill = (SolidColorBrush)Application.Current.Resources["ApplicationIgnoredBrush"];
                    });

                    //Request to stop video capture
                    CaptureResult videoResult = CaptureImport.CaptureVideoStop();
                    Debug.WriteLine("Capture video stop result: " + videoResult.Status + " / " + videoResult.Message);
                    return videoResult;
                }
                else
                {
                    return new CaptureResult() { Status = CaptureStatus.Failed, Message = "No video capture active" };
                }
            }
            catch (Exception ex)
            {
                Debug.WriteLine("Screen video capture stop failed: " + ex.Message);
                return new CaptureResult() { Status = CaptureStatus.Failed, Message = "Failed: " + ex.Message };
            }
        }

        public static async void CaptureEventVideoCaptureStopped()
        {
            try
            {
                Debug.WriteLine("Video capture stopped event triggered, closing application.");
                await AppExit.Exit();
            }
            catch { }
        }

        public static async void CaptureEventDeviceChangeDetected()
        {
            try
            {
                Debug.WriteLine("Device change event triggered, stopping capture and closing application.");
                await AppExit.Exit();
            }
            catch { }
        }

        public static CaptureResult StartCaptureVideoToFile()
        {
            try
            {
                //Check video capture
                if (CaptureImport.CaptureVideoIsRecording())
                {
                    return new CaptureResult() { Status = CaptureStatus.Busy, Message = "Already recording video" };
                }

                //Register capture events
                CaptureImport.CaptureEventVideoCaptureStopped(CaptureEventVideoCaptureStopped);
                CaptureImport.CaptureEventDeviceChangeDetected(CaptureEventDeviceChangeDetected);

                //Capture tool settings
                VideoFormats VideoSaveFormat = (VideoFormats)SettingLoad(vConfigurationScreenCaptureTool, "VideoSaveFormat", typeof(int));
                VideoRateControls VideoRateControl = (VideoRateControls)SettingLoad(vConfigurationScreenCaptureTool, "VideoRateControl", typeof(int));
                int VideoBitRate = SettingLoad(vConfigurationScreenCaptureTool, "VideoBitRate", typeof(int));
                int VideoFrameRate = SettingLoad(vConfigurationScreenCaptureTool, "VideoFrameRate", typeof(int));
                int VideoMaxPixelDimension = SettingLoad(vConfigurationScreenCaptureTool, "VideoMaxPixelDimension", typeof(int));

                AudioFormats AudioSaveFormat = (AudioFormats)SettingLoad(vConfigurationScreenCaptureTool, "AudioSaveFormat", typeof(int));
                int AudioChannels = SettingLoad(vConfigurationScreenCaptureTool, "AudioChannels", typeof(int));
                int AudioBitRate = SettingLoad(vConfigurationScreenCaptureTool, "AudioBitRate", typeof(int));
                int AudioBitDepth = SettingLoad(vConfigurationScreenCaptureTool, "AudioBitDepth", typeof(int));
                int AudioSampleRate = SettingLoad(vConfigurationScreenCaptureTool, "AudioSampleRate", typeof(int));

                int CaptureMonitorId = SettingLoad(vConfigurationScreenCaptureTool, "CaptureMonitorId", typeof(int)) - 1;
                bool CaptureSoundEffect = SettingLoad(vConfigurationScreenCaptureTool, "CaptureSoundEffect", typeof(bool));
                bool CaptureDrawBorder = SettingLoad(vConfigurationScreenCaptureTool, "CaptureDrawBorder", typeof(bool));
                bool CaptureDrawMouseCursor = SettingLoad(vConfigurationScreenCaptureTool, "CaptureDrawMouseCursor", typeof(bool));

                //Screen capture settings
                CaptureSettings captureSettings = new CaptureSettings();
                captureSettings.HDRtoSDR = true;
                captureSettings.MaxPixelDimension = VideoMaxPixelDimension;
                captureSettings.MonitorId = CaptureMonitorId;
                captureSettings.SoundEffect = CaptureSoundEffect;
                captureSettings.DrawBorder = CaptureDrawBorder;
                captureSettings.DrawMouseCursor = CaptureDrawMouseCursor;

                //Media recording settings
                MediaSettings mediaSettings = new MediaSettings();
                mediaSettings.VideoFormat = VideoSaveFormat;
                mediaSettings.VideoFrameRate = VideoFrameRate;
                mediaSettings.VideoRateControl = VideoRateControl;
                mediaSettings.VideoBitRate = VideoBitRate;
                mediaSettings.AudioFormat = AudioSaveFormat;
                mediaSettings.AudioChannels = AudioChannels;
                mediaSettings.AudioBitRate = AudioBitRate;
                mediaSettings.AudioBitDepth = AudioBitDepth;
                mediaSettings.AudioSampleRate = AudioSampleRate;

                //Initialize screen capture
                CaptureResult captureResult = CaptureImport.CaptureInitialize(captureSettings, true);
                Debug.WriteLine("Capture initialize result: " + captureResult.Status + " / " + captureResult.ResultCode + " / " + captureResult.Message);
                if (captureResult.Status == CaptureStatus.Success)
                {
                    vCaptureDetails = CaptureImport.CaptureGetDetails();
                }
                else
                {
                    return captureResult;
                }

                //Set save name
                string fileSaveName = "Video";
                if (SettingLoad(vConfigurationScreenCaptureTool, "SaveWindowTitle", typeof(bool)))
                {
                    fileSaveName = Detail_WindowTitleByWindowHandle(GetForegroundWindow());
                    fileSaveName = AVFunctions.StringCut(fileSaveName, 150, string.Empty);
                }

                //Set save date
                string fileSaveDate = " (" + DateTime.Now.ToShortDateString() + ") " + DateTime.Now.ToString("HH.mm.ss.ffff");

                //Set save details
                string fileSaveDetails = string.Empty;
                if (vCaptureDetails.HDREnabled)
                {
                    if (vCaptureDetails.HDRtoSDR)
                    {
                        fileSaveDetails = " (HDRtoSDR)";
                    }
                    else
                    {
                        fileSaveDetails = " (HDR)";
                    }
                }
                else
                {
                    fileSaveDetails = " (SDR)";
                }

                //Replace invalid characters
                vCaptureFileName = AVFiles.FileNameReplaceInvalidChars(fileSaveName + fileSaveDate + fileSaveDetails, "-");

                //Check capture location
                string fileSaveFolder = SettingLoad(vConfigurationScreenCaptureTool, "CaptureLocation", typeof(string));
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
                string fileSavePath = fileSaveFolder + "\\" + vCaptureFileName + ".mp4";

                //Start video capture
                CaptureResult videoResult = CaptureImport.CaptureVideoStart(fileSavePath, mediaSettings);
                Debug.WriteLine("Capture video start result: " + videoResult.Status + " / " + videoResult.ResultCode + " / " + videoResult.Message);
                return videoResult;
            }
            catch (Exception ex)
            {
                Debug.WriteLine("Screen video capture start failed: " + ex.Message);
                return new CaptureResult() { Status = CaptureStatus.Failed, Message = "Failed: " + ex.Message };
            }
        }
    }
}