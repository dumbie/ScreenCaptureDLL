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
                ImageFormats imageSaveFormat = (ImageFormats)SettingLoad(vConfiguration, "ImageSaveFormat", typeof(int));
                int imageSaveQuality = SettingLoad(vConfiguration, "ImageSaveQuality", typeof(int));

                //Screen capture settings
                CaptureSettings captureSettings = new CaptureSettings();

                //Check HDR to SDR setting
                if (imageSaveFormat == ImageFormats.JPG || imageSaveFormat == ImageFormats.PNG || imageSaveFormat == ImageFormats.BMP || imageSaveFormat == ImageFormats.TIF || imageSaveFormat == ImageFormats.HEIF)
                {
                    captureSettings.HDRtoSDR = true;
                }

                //Initialize screen capture
                if (!CaptureImport.CaptureInitialize(captureSettings, out CaptureDetails vCaptureDetails))
                {
                    Debug.WriteLine("Failed to initialize screen capture.");

                    //Play capture sound
                    PlayCaptureSound("CaptureFailed");
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
                    if (captureSettings.HDRtoSDR)
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
                string fileSaveFolder = SettingLoad(vConfiguration, "ScreenshotLocation", typeof(string));
                if (string.IsNullOrWhiteSpace(fileSaveFolder) || !Directory.Exists(fileSaveFolder))
                {
                    //Check screenshots folder in app directory
                    if (!Directory.Exists("Screenshots"))
                    {
                        Directory.CreateDirectory("Screenshots");
                    }

                    //Set save folder to screenshots in app directory
                    fileSaveFolder = "Screenshots";
                }

                //Combine save path
                string fileSavePath = fileSaveFolder + fileSaveName;

                //Save screenshot to file
                bool screenshotSaved = false;
                if (imageSaveFormat == ImageFormats.JPG)
                {
                    screenshotSaved = CaptureImport.CaptureImage(fileSavePath + ".jpg", imageSaveQuality, imageSaveFormat);
                    Debug.WriteLine("Screenshot JPG export succeeded: " + screenshotSaved);
                }
                else if (imageSaveFormat == ImageFormats.PNG)
                {
                    screenshotSaved = CaptureImport.CaptureImage(fileSavePath + ".png", 100, imageSaveFormat);
                    Debug.WriteLine("Screenshot PNG export succeeded: " + screenshotSaved);
                }
                else if (imageSaveFormat == ImageFormats.BMP)
                {
                    screenshotSaved = CaptureImport.CaptureImage(fileSavePath + ".bmp", 100, imageSaveFormat);
                    Debug.WriteLine("Screenshot BMP export succeeded: " + screenshotSaved);
                }
                else if (imageSaveFormat == ImageFormats.TIF)
                {
                    screenshotSaved = CaptureImport.CaptureImage(fileSavePath + ".tif", 100, imageSaveFormat);
                    Debug.WriteLine("Screenshot TIF export succeeded: " + screenshotSaved);
                }
                else if (imageSaveFormat == ImageFormats.HEIF)
                {
                    screenshotSaved = CaptureImport.CaptureImage(fileSavePath + ".heif", imageSaveQuality, imageSaveFormat);
                    Debug.WriteLine("Screenshot HEIF export succeeded: " + screenshotSaved);
                }
                else
                {
                    screenshotSaved = CaptureImport.CaptureImage(fileSavePath + ".jxr", imageSaveQuality, imageSaveFormat);
                    Debug.WriteLine("Screenshot JXR export succeeded: " + screenshotSaved);
                }

                //Play capture sound
                if (screenshotSaved)
                {
                    PlayCaptureSound("CaptureScreenshot");
                }
                else
                {
                    PlayCaptureSound("CaptureFailed");
                }
            }
            catch (Exception ex)
            {
                Debug.WriteLine("Screen image capture failed: " + ex.Message);

                //Play capture sound
                PlayCaptureSound("CaptureFailed");
            }
            finally
            {
                //Reset screen capture resources
                CaptureImport.CaptureReset();
            }
        }
    }
}