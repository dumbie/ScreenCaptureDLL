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
            IntPtr bitmapIntPtr = IntPtr.Zero;
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
                    PlayCaptureSound(true);
                    return;
                }
                else
                {
                    //Wait for capturer to have initialized
                    await Task.Delay(500);
                }

                //Capture screenshot
                try
                {
                    bitmapIntPtr = CaptureImport.CaptureScreenshot();
                }
                catch { }

                //Check screenshot
                if (bitmapIntPtr == IntPtr.Zero)
                {
                    Debug.WriteLine("Screenshot capture is corrupted.");

                    //Play capture sound
                    PlayCaptureSound(true);
                    return;
                }

                //Set file name
                string imageSaveName = "(" + DateTime.Now.ToShortDateString() + ") " + DateTime.Now.ToString("HH.mm.ss.ffff");
                if (vCaptureDetails.HDREnabled)
                {
                    if (captureSettings.HDRtoSDR)
                    {
                        imageSaveName += " (HDRtoSDR)";
                    }
                    else
                    {
                        imageSaveName += " (HDR)";
                    }
                }
                else
                {
                    imageSaveName += " (SDR)";
                }
                imageSaveName = "\\Screenshot " + AVFiles.FileNameReplaceInvalidChars(imageSaveName, "-");

                //Check screenshot location
                string screenshotSaveFolder = SettingLoad(vConfiguration, "ScreenshotLocation", typeof(string));
                if (string.IsNullOrWhiteSpace(screenshotSaveFolder) || !Directory.Exists(screenshotSaveFolder))
                {
                    //Check screenshots folder in app directory
                    if (!Directory.Exists("Screenshots"))
                    {
                        Directory.CreateDirectory("Screenshots");
                    }

                    //Set save folder to screenshots in app directory
                    screenshotSaveFolder = "Screenshots";
                }

                //Save screenshot to file
                bool screenshotSaved = false;
                if (imageSaveFormat == ImageFormats.JPG)
                {
                    screenshotSaved = CaptureImport.CaptureImage(bitmapIntPtr, screenshotSaveFolder + imageSaveName + ".jpg", imageSaveQuality, imageSaveFormat);
                    Debug.WriteLine("Screenshot JPG export succeeded: " + screenshotSaved);
                }
                else if (imageSaveFormat == ImageFormats.PNG)
                {
                    screenshotSaved = CaptureImport.CaptureImage(bitmapIntPtr, screenshotSaveFolder + imageSaveName + ".png", 100, imageSaveFormat);
                    Debug.WriteLine("Screenshot PNG export succeeded: " + screenshotSaved);
                }
                else if (imageSaveFormat == ImageFormats.BMP)
                {
                    screenshotSaved = CaptureImport.CaptureImage(bitmapIntPtr, screenshotSaveFolder + imageSaveName + ".bmp", 100, imageSaveFormat);
                    Debug.WriteLine("Screenshot BMP export succeeded: " + screenshotSaved);
                }
                else if (imageSaveFormat == ImageFormats.TIF)
                {
                    screenshotSaved = CaptureImport.CaptureImage(bitmapIntPtr, screenshotSaveFolder + imageSaveName + ".tif", 100, imageSaveFormat);
                    Debug.WriteLine("Screenshot TIF export succeeded: " + screenshotSaved);
                }
                else if (imageSaveFormat == ImageFormats.HEIF)
                {
                    screenshotSaved = CaptureImport.CaptureImage(bitmapIntPtr, screenshotSaveFolder + imageSaveName + ".heif", imageSaveQuality, imageSaveFormat);
                    Debug.WriteLine("Screenshot HEIF export succeeded: " + screenshotSaved);
                }
                else
                {
                    screenshotSaved = CaptureImport.CaptureImage(bitmapIntPtr, screenshotSaveFolder + imageSaveName + ".jxr", imageSaveQuality, imageSaveFormat);
                    Debug.WriteLine("Screenshot JXR export succeeded: " + screenshotSaved);
                }

                //Play capture sound
                if (screenshotSaved)
                {
                    PlayCaptureSound(false);
                }
                else
                {
                    PlayCaptureSound(true);
                }
            }
            catch (Exception ex)
            {
                Debug.WriteLine("Screen capture failed: " + ex.Message);

                //Play capture sound
                PlayCaptureSound(true);
            }
            finally
            {
                //Clear screen capture resources
                if (bitmapIntPtr != IntPtr.Zero)
                {
                    CaptureImport.CaptureFreeMemory(bitmapIntPtr);
                }

                //Reset screen capture resources
                CaptureImport.CaptureReset();
            }
        }
    }
}