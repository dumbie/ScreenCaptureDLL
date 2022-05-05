using System;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Media;

namespace ScreenCapture
{
    public class CaptureScreen
    {
        //Player Variables
        private static MediaPlayer vWindowsMediaPlayer = new MediaPlayer();

        //Screen Capture Variables
        public static CaptureDetails vCaptureDetails;
        public static CaptureSettings vCaptureSettings = new CaptureSettings();

        //Setting Variables
        public static string vSettingSoundFolder = "Resources";
        public static string vSettingImageFormat = "png";
        public static string vSettingImageSaveFolder = string.Empty;
        public static string vSettingImageSaveName = string.Empty;
        public static int vSettingImageQuality = 100;

        public static async Task CaptureScreenToFile(string[] launchArgs)
        {
            //Set launch arguments
            if (launchArgs != null && launchArgs.Any())
            {
                foreach (string launchArgument in launchArgs)
                {
                    try
                    {
                        //Convert launch argument to lower
                        string launchArgumentLower = launchArgument.ToLower();

                        //MonitorId
                        if (launchArgumentLower.StartsWith("-monitorid"))
                        {
                            vCaptureSettings.MonitorId = Convert.ToInt32(Regex.Replace(launchArgumentLower, "(-monitorid).{1}", string.Empty));
                        }

                        //ImageFormat
                        if (launchArgumentLower.StartsWith("-imageformat"))
                        {
                            vSettingImageFormat = Regex.Replace(launchArgumentLower, "(-imageformat).{1}", string.Empty);
                        }

                        //ImageQuality
                        if (launchArgumentLower.StartsWith("-imagequality"))
                        {
                            vSettingImageQuality = Convert.ToInt32(Regex.Replace(launchArgumentLower, "(-imagequality).{1}", string.Empty));
                        }

                        //ImageSaveFolder
                        if (launchArgumentLower.StartsWith("-imagesavefolder"))
                        {
                            vSettingImageSaveFolder = Regex.Replace(launchArgumentLower, "(-imagesavefolder).{1}", string.Empty).Replace("\"", string.Empty);
                        }

                        //SoundFolder
                        if (launchArgumentLower.StartsWith("-soundfolder"))
                        {
                            vSettingSoundFolder = Regex.Replace(launchArgumentLower, "(-soundfolder).{1}", string.Empty).Replace("\"", string.Empty);
                        }

                        //MaxPixelDimension
                        if (launchArgumentLower.StartsWith("-maxpixeldimension"))
                        {
                            vCaptureSettings.MaxPixelDimension = Convert.ToInt32(Regex.Replace(launchArgumentLower, "(-maxpixeldimension).{1}", string.Empty));
                        }

                        //HDRtoSDR
                        if (launchArgumentLower.StartsWith("-hdrtosdr"))
                        {
                            vCaptureSettings.HDRtoSDR = Convert.ToBoolean(Regex.Replace(launchArgumentLower, "(-hdrtosdr).{1}", string.Empty));
                        }

                        //HDRBrightness
                        if (launchArgumentLower.StartsWith("-hdrbrightness"))
                        {
                            vCaptureSettings.HDRBrightness = Convert.ToSingle(Regex.Replace(launchArgumentLower, "(-hdrbrightness).{1}", string.Empty), CultureInfo.InvariantCulture);
                        }

                        //Saturation
                        if (launchArgumentLower.StartsWith("-saturation"))
                        {
                            vCaptureSettings.Saturation = Convert.ToSingle(Regex.Replace(launchArgumentLower, "(-saturation).{1}", string.Empty), CultureInfo.InvariantCulture);
                        }

                        //RedChannel
                        if (launchArgumentLower.StartsWith("-redchannel"))
                        {
                            vCaptureSettings.RedChannel = Convert.ToSingle(Regex.Replace(launchArgumentLower, "(-redchannel).{1}", string.Empty), CultureInfo.InvariantCulture);
                        }

                        //GreenChannel
                        if (launchArgumentLower.StartsWith("-greenchannel"))
                        {
                            vCaptureSettings.GreenChannel = Convert.ToSingle(Regex.Replace(launchArgumentLower, "(-greenchannel).{1}", string.Empty), CultureInfo.InvariantCulture);
                        }

                        //BlueChannel
                        if (launchArgumentLower.StartsWith("-bluechannel"))
                        {
                            vCaptureSettings.BlueChannel = Convert.ToSingle(Regex.Replace(launchArgumentLower, "(-bluechannel).{1}", string.Empty), CultureInfo.InvariantCulture);
                        }

                        //Brightness
                        if (launchArgumentLower.StartsWith("-brightness"))
                        {
                            vCaptureSettings.Brightness = Convert.ToSingle(Regex.Replace(launchArgumentLower, "(-brightness).{1}", string.Empty), CultureInfo.InvariantCulture);
                        }

                        //Contrast
                        if (launchArgumentLower.StartsWith("-contrast"))
                        {
                            vCaptureSettings.Contrast = Convert.ToSingle(Regex.Replace(launchArgumentLower, "(-contrast).{1}", string.Empty), CultureInfo.InvariantCulture);
                        }

                        //Gamma
                        if (launchArgumentLower.StartsWith("-gamma"))
                        {
                            vCaptureSettings.Gamma = Convert.ToSingle(Regex.Replace(launchArgumentLower, "(-gamma).{1}", string.Empty), CultureInfo.InvariantCulture);
                        }
                    }
                    catch { }
                }
            }

            //Prepare screen capture
            IntPtr bitmapIntPtr = IntPtr.Zero;
            try
            {
                //Initialize screen capture
                if (!CaptureImport.CaptureInitialize(vCaptureSettings, out CaptureDetails vCaptureDetails))
                {
                    Debug.WriteLine("Failed to initialize screen capture.");

                    //Play capture sound
                    vWindowsMediaPlayer.Volume = 1.0;
                    vWindowsMediaPlayer.Open(new Uri(vSettingSoundFolder + "\\ScreenshotFail.mp3", UriKind.RelativeOrAbsolute));
                    vWindowsMediaPlayer.Play();

                    //Allow sound to finish
                    await Task.Delay(1000);
                    return;
                }
                else
                {
                    //Set capture details string
                    string captureDetails = "Width: " + vCaptureDetails.Width;
                    captureDetails += "\nHeight: " + vCaptureDetails.Height;
                    captureDetails += "\nPixelByteSize: " + vCaptureDetails.PixelByteSize;
                    captureDetails += "\nWidthByteSize: " + vCaptureDetails.WidthByteSize;
                    captureDetails += "\nTotalByteSize: " + vCaptureDetails.TotalByteSize;
                    captureDetails += "\nHDREnabled: " + vCaptureDetails.HDREnabled;
                    captureDetails += "\nHDRtoSDR: " + vCaptureSettings.HDRtoSDR;
                    captureDetails += "\nSDRWhiteLevel: " + vCaptureDetails.SDRWhiteLevel;

                    //Write capture details
                    Debug.WriteLine(captureDetails);

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
                    vWindowsMediaPlayer.Volume = 1.0;
                    vWindowsMediaPlayer.Open(new Uri(vSettingSoundFolder + "\\ScreenshotFail.mp3", UriKind.RelativeOrAbsolute));
                    vWindowsMediaPlayer.Play();

                    //Allow sound to finish
                    await Task.Delay(1000);
                    return;
                }

                //Set file name
                vSettingImageSaveName = "\\Screenshot " + DateTime.Now.ToString("HH.mm.ss.ffff") + " (" + DateTime.Now.ToShortDateString() + ")";
                if (vCaptureDetails.HDREnabled)
                {
                    if (vCaptureSettings.HDRtoSDR)
                    {
                        vSettingImageSaveName += " (HDRtoSDR)";
                    }
                    else
                    {
                        vSettingImageSaveName += " (HDR)";
                    }
                }
                else
                {
                    vSettingImageSaveName += " (SDR)";
                }

                //Set folder path
                if (string.IsNullOrWhiteSpace(vSettingImageSaveFolder) || !Directory.Exists(vSettingImageSaveFolder))
                {
                    //Create screenshots folder in app directory
                    if (!Directory.Exists("Screenshots"))
                    {
                        Directory.CreateDirectory("Screenshots");
                    }

                    //Set save folder to screenshots in app directory
                    vSettingImageSaveFolder = "Screenshots";
                }

                //Save screenshot to file
                bool screenshotExport = false;
                if (vSettingImageFormat == "bmp")
                {
                    screenshotExport = CaptureImport.CaptureSaveFileBmp(bitmapIntPtr, vSettingImageSaveFolder + vSettingImageSaveName + ".bmp");
                    Debug.WriteLine("Screenshot bmp export succeeded: " + screenshotExport);
                }
                else if (vSettingImageFormat == "jpg")
                {
                    screenshotExport = CaptureImport.CaptureSaveFileJpg(bitmapIntPtr, vSettingImageSaveFolder + vSettingImageSaveName + ".jpg", vSettingImageQuality);
                    Debug.WriteLine("Screenshot jpg export succeeded: " + screenshotExport);
                }
                else if (vSettingImageFormat == "png")
                {
                    screenshotExport = CaptureImport.CaptureSaveFilePng(bitmapIntPtr, vSettingImageSaveFolder + vSettingImageSaveName + ".png");
                    Debug.WriteLine("Screenshot png export succeeded: " + screenshotExport);
                }
                else if (vSettingImageFormat == "jxr")
                {
                    screenshotExport = CaptureImport.CaptureSaveFileJxr(bitmapIntPtr, vSettingImageSaveFolder + vSettingImageSaveName + ".jxr");
                    Debug.WriteLine("Screenshot jxr export succeeded: " + screenshotExport);
                }

                //Play capture sound
                if (screenshotExport)
                {
                    vWindowsMediaPlayer.Volume = 1.0;
                    vWindowsMediaPlayer.Open(new Uri(vSettingSoundFolder + "\\Screenshot.mp3", UriKind.RelativeOrAbsolute));
                    vWindowsMediaPlayer.Play();
                }
                else
                {
                    vWindowsMediaPlayer.Volume = 1.0;
                    vWindowsMediaPlayer.Open(new Uri(vSettingSoundFolder + "\\ScreenshotFail.mp3", UriKind.RelativeOrAbsolute));
                    vWindowsMediaPlayer.Play();
                }

                //Allow sound to finish
                await Task.Delay(1000);
            }
            catch (Exception ex)
            {
                Debug.WriteLine("Screen capture failed: " + ex.Message);
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

                //Exit application
                Application.Current.Shutdown();
            }
        }
    }
}