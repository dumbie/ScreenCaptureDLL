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
        //Prepare image capture
        public static async Task CaptureImageToFile()
        {
            try
            {
                //Capture tool settings
                ImageFormats ScreenshotSaveFormat = (ImageFormats)SettingLoad(vConfiguration, "ScreenshotSaveFormat", typeof(int));
                int ScreenshotSaveQuality = SettingLoad(vConfiguration, "ScreenshotSaveQuality", typeof(int));
                int ScreenshotMaxPixelDimension = SettingLoad(vConfiguration, "ScreenshotMaxPixelDimension", typeof(int));
                int CaptureMonitorId = SettingLoad(vConfiguration, "CaptureMonitorId", typeof(int)) - 1;
                bool CaptureSoundEffect = SettingLoad(vConfiguration, "CaptureSoundEffect", typeof(bool));
                bool CaptureMouseDrawCursor = SettingLoad(vConfiguration, "CaptureMouseDrawCursor", typeof(bool));

                //Screen capture settings
                CaptureSettings captureSettings = new CaptureSettings();
                captureSettings.MonitorId = CaptureMonitorId;
                captureSettings.MaxPixelDimension = ScreenshotMaxPixelDimension;
                captureSettings.SoundEffect = CaptureSoundEffect;
                captureSettings.MouseDrawCursor = CaptureMouseDrawCursor;

                //Check HDR to SDR setting
                if (ScreenshotSaveFormat == ImageFormats.JPG || ScreenshotSaveFormat == ImageFormats.PNG || ScreenshotSaveFormat == ImageFormats.BMP || ScreenshotSaveFormat == ImageFormats.TIF || ScreenshotSaveFormat == ImageFormats.HEIF)
                {
                    captureSettings.HDRtoSDR = true;
                }

                //Initialize screen capture
                if (!CaptureImport.CaptureInitialize(0, captureSettings, out CaptureDetails vCaptureDetails, false))
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
                fileSaveName = "\\Screenshot " + AVFiles.FileNameReplaceInvalidChars(fileSaveName, "-");

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
                string fileSavePath = fileSaveFolder + fileSaveName;

                //Save screenshot to file
                bool screenshotSaved = false;
                if (ScreenshotSaveFormat == ImageFormats.JPG)
                {
                    screenshotSaved = CaptureImport.CaptureImage(0, fileSavePath + ".jpg", ScreenshotSaveQuality, ScreenshotSaveFormat);
                    Debug.WriteLine("Screenshot JPG export succeeded: " + screenshotSaved);
                }
                else if (ScreenshotSaveFormat == ImageFormats.PNG)
                {
                    screenshotSaved = CaptureImport.CaptureImage(0, fileSavePath + ".png", 100, ScreenshotSaveFormat);
                    Debug.WriteLine("Screenshot PNG export succeeded: " + screenshotSaved);
                }
                else if (ScreenshotSaveFormat == ImageFormats.BMP)
                {
                    screenshotSaved = CaptureImport.CaptureImage(0, fileSavePath + ".bmp", 100, ScreenshotSaveFormat);
                    Debug.WriteLine("Screenshot BMP export succeeded: " + screenshotSaved);
                }
                else if (ScreenshotSaveFormat == ImageFormats.TIF)
                {
                    screenshotSaved = CaptureImport.CaptureImage(0, fileSavePath + ".tif", 100, ScreenshotSaveFormat);
                    Debug.WriteLine("Screenshot TIF export succeeded: " + screenshotSaved);
                }
                else if (ScreenshotSaveFormat == ImageFormats.HEIF)
                {
                    screenshotSaved = CaptureImport.CaptureImage(0, fileSavePath + ".heif", ScreenshotSaveQuality, ScreenshotSaveFormat);
                    Debug.WriteLine("Screenshot HEIF export succeeded: " + screenshotSaved);
                }
                else
                {
                    screenshotSaved = CaptureImport.CaptureImage(0, fileSavePath + ".jxr", ScreenshotSaveQuality, ScreenshotSaveFormat);
                    Debug.WriteLine("Screenshot JXR export succeeded: " + screenshotSaved);
                }
            }
            catch (Exception ex)
            {
                Debug.WriteLine("Screen image capture failed: " + ex.Message);
            }
            finally
            {
                //Reset screen capture resources
                CaptureImport.CaptureReset(0);
            }
        }
    }
}