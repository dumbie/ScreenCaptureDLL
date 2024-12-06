using ArnoldVinkCode;
using ScreenCaptureImport;
using System;
using System.Diagnostics;
using System.IO;
using System.Threading.Tasks;
using static ArnoldVinkCode.AVInteropDll;
using static ArnoldVinkCode.AVProcess;
using static ArnoldVinkCode.AVSettings;
using static ScreenCapture.AppVariables;

namespace ScreenCapture
{
    public partial class CaptureScreen
    {
        public static async Task CaptureImageProcess(int captureDelay)
        {
            try
            {
                //Capture start stop delay
                await Task.Delay(captureDelay);

                //Check active video capture
                if (!CaptureImport.CaptureVideoIsRecording())
                {
                    Debug.WriteLine("Starting image capture process.");
                    Process startProcess = new Process();
                    startProcess.StartInfo.FileName = "ScreenCaptureTool.exe";
                    startProcess.StartInfo.Arguments = "-image";
                    startProcess.Start();
                }
            }
            catch (Exception ex)
            {
                Debug.WriteLine("Screen image capture process start failed: " + ex.Message);
            }
        }

        public static bool CaptureImageToFile()
        {
            try
            {
                //Capture tool settings
                ImageFormats ScreenshotSaveFormat = (ImageFormats)SettingLoad(vConfigurationScreenCaptureTool, "ScreenshotSaveFormat", typeof(int));
                int ScreenshotSaveQuality = SettingLoad(vConfigurationScreenCaptureTool, "ScreenshotSaveQuality", typeof(int));
                int ScreenshotMaxPixelDimension = SettingLoad(vConfigurationScreenCaptureTool, "ScreenshotMaxPixelDimension", typeof(int));
                int CaptureMonitorId = SettingLoad(vConfigurationScreenCaptureTool, "CaptureMonitorId", typeof(int)) - 1;
                bool CaptureSoundEffect = SettingLoad(vConfigurationScreenCaptureTool, "CaptureSoundEffect", typeof(bool));
                bool CaptureDrawBorder = SettingLoad(vConfigurationScreenCaptureTool, "CaptureDrawBorder", typeof(bool));
                bool CaptureDrawMouseCursor = SettingLoad(vConfigurationScreenCaptureTool, "CaptureDrawMouseCursor", typeof(bool));

                //Screen capture settings
                CaptureSettings captureSettings = new CaptureSettings();
                captureSettings.MonitorId = CaptureMonitorId;
                captureSettings.MaxPixelDimension = ScreenshotMaxPixelDimension;
                captureSettings.SoundEffect = CaptureSoundEffect;
                captureSettings.DrawBorder = CaptureDrawBorder;
                captureSettings.DrawMouseCursor = CaptureDrawMouseCursor;

                //Check HDR to SDR setting
                if (ScreenshotSaveFormat == ImageFormats.JPG || ScreenshotSaveFormat == ImageFormats.PNG || ScreenshotSaveFormat == ImageFormats.BMP || ScreenshotSaveFormat == ImageFormats.TIF || ScreenshotSaveFormat == ImageFormats.HEIF)
                {
                    captureSettings.HDRtoSDR = true;
                }

                //Initialize screen capture
                CaptureStatus captureInitialized = CaptureImport.CaptureInitialize(captureSettings, true);
                if (captureInitialized == CaptureStatus.Initialized)
                {
                    vCaptureDetails = CaptureImport.CaptureGetDetails();
                }
                else if (captureInitialized == CaptureStatus.Busy)
                {
                    Debug.WriteLine("Already initializing screen capture.");
                    return false;
                }
                else
                {
                    Debug.WriteLine("Failed to initialize screen capture.");
                    return false;
                }

                //Set save name
                string fileSaveName = "Screenshot";
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
                string fileSavePath = fileSaveFolder + "\\" + vCaptureFileName;

                //Save screenshot to file
                bool screenshotSaved = false;
                if (ScreenshotSaveFormat == ImageFormats.JPG)
                {
                    screenshotSaved = CaptureImport.CaptureImage(fileSavePath + ".jpg", ScreenshotSaveQuality, ScreenshotSaveFormat);
                    Debug.WriteLine("Screenshot JPG export succeeded: " + screenshotSaved);
                }
                else if (ScreenshotSaveFormat == ImageFormats.PNG)
                {
                    screenshotSaved = CaptureImport.CaptureImage(fileSavePath + ".png", 100, ScreenshotSaveFormat);
                    Debug.WriteLine("Screenshot PNG export succeeded: " + screenshotSaved);
                }
                else if (ScreenshotSaveFormat == ImageFormats.BMP)
                {
                    screenshotSaved = CaptureImport.CaptureImage(fileSavePath + ".bmp", 100, ScreenshotSaveFormat);
                    Debug.WriteLine("Screenshot BMP export succeeded: " + screenshotSaved);
                }
                else if (ScreenshotSaveFormat == ImageFormats.TIF)
                {
                    screenshotSaved = CaptureImport.CaptureImage(fileSavePath + ".tif", 100, ScreenshotSaveFormat);
                    Debug.WriteLine("Screenshot TIF export succeeded: " + screenshotSaved);
                }
                else if (ScreenshotSaveFormat == ImageFormats.HEIF)
                {
                    screenshotSaved = CaptureImport.CaptureImage(fileSavePath + ".heif", ScreenshotSaveQuality, ScreenshotSaveFormat);
                    Debug.WriteLine("Screenshot HEIF export succeeded: " + screenshotSaved);
                }
                else
                {
                    screenshotSaved = CaptureImport.CaptureImage(fileSavePath + ".jxr", ScreenshotSaveQuality, ScreenshotSaveFormat);
                    Debug.WriteLine("Screenshot JXR export succeeded: " + screenshotSaved);
                }
                return screenshotSaved;
            }
            catch (Exception ex)
            {
                Debug.WriteLine("Screen image capture failed: " + ex.Message);
                return false;
            }
        }
    }
}