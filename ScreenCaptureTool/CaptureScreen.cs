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
        public static async Task CaptureScreenToFile()
        {
            //Prepare screen capture
            IntPtr bitmapIntPtr = IntPtr.Zero;
            try
            {
                //Screen capture settings
                CaptureSettings vCaptureSettings = new CaptureSettings();
                vCaptureSettings.HDRtoSDR = SettingLoad(vConfiguration, "ScreenshotHDRtoSDR", typeof(bool));

                //Initialize screen capture
                if (!CaptureImport.CaptureInitialize(vCaptureSettings, out CaptureDetails vCaptureDetails))
                {
                    Debug.WriteLine("Failed to initialize screen capture.");

                    //Play capture sound
                    CaptureSound(true);

                    return;
                }
                else
                {
                    //Wait for capturer to have initialized
                    await Task.Delay(100);
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
                    CaptureSound(true);

                    return;
                }

                //Set file name
                string imageSaveName = "(" + DateTime.Now.ToShortDateString() + ") " + DateTime.Now.ToString("HH.mm.ss.ffff");
                if (vCaptureDetails.HDREnabled)
                {
                    if (vCaptureSettings.HDRtoSDR)
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
                imageSaveName = "\\Screenshot " + CaptureFunctions.FileNameReplaceInvalidChars(imageSaveName);

                //Check screenshot location
                string screenshotSaveFolder = SettingLoad(vConfiguration, "ScreenshotLocation", typeof(string));
                if (!Directory.Exists(screenshotSaveFolder))
                {
                    //Check screenshots folder in app directory
                    if (!Directory.Exists("Screenshots"))
                    {
                        Directory.CreateDirectory("Screenshots");
                    }
                    screenshotSaveFolder = "Screenshots";
                }

                //Save screenshot to file
                bool screenshotExport = false;
                if (vCaptureDetails.HDREnabled && !vCaptureSettings.HDRtoSDR)
                {
                    screenshotExport = CaptureImport.CaptureSaveFileJxr(bitmapIntPtr, screenshotSaveFolder + imageSaveName + ".jxr");
                    Debug.WriteLine("Screenshot jxr export succeeded: " + screenshotExport);
                }
                else
                {
                    screenshotExport = CaptureImport.CaptureSaveFilePng(bitmapIntPtr, screenshotSaveFolder + imageSaveName + ".png");
                    Debug.WriteLine("Screenshot png export succeeded: " + screenshotExport);
                }

                //Play capture sound
                if (screenshotExport)
                {
                    //Play capture sound
                    CaptureSound(false);
                }
                else
                {
                    //Play capture sound
                    CaptureSound(true);
                }
            }
            catch (Exception ex)
            {
                Debug.WriteLine("Screen capture failed: " + ex.Message);

                //Play capture sound
                CaptureSound(true);
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