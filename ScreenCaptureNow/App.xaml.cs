using System;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Media;

namespace ScreenCapture
{
    public partial class App : Application
    {
        //Player Variables
        private static MediaPlayer vWindowsMediaPlayer = new MediaPlayer();

        //Screen Capture Variables
        public static CaptureDetails vCaptureDetails;
        public static CaptureSettings vCaptureSettings = new CaptureSettings();

        //Setting Variables
        public string vSettingImageFormat = "png";
        public int vSettingImageQuality = 100;

        protected override async void OnStartup(StartupEventArgs e)
        {
            //Set launch arguments
            if (e.Args.Any())
            {
                foreach (string launchArgument in e.Args)
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

            //Set the working directory to executable directory
            Directory.SetCurrentDirectory(Path.GetDirectoryName(Assembly.GetEntryAssembly().Location));

            //Create screenshots export directory
            if (!Directory.Exists("Screenshots"))
            {
                Directory.CreateDirectory("Screenshots");
            }

            //Prepare screen capture
            IntPtr bitmapIntPtr = IntPtr.Zero;
            try
            {
                //Initialize screen capture
                await InitializeScreenCapture(500);

                //Capture screenshot
                try
                {
                    bitmapIntPtr = CaptureImport.CaptureScreenshot();
                }
                catch { }

                //Check screenshot
                if (bitmapIntPtr == IntPtr.Zero)
                {
                    Debug.WriteLine("Screenshot is corrupted.");

                    //Play capture sound
                    vWindowsMediaPlayer.Volume = 1.0;
                    vWindowsMediaPlayer.Open(new Uri("Resources\\ScreenshotFail.mp3", UriKind.RelativeOrAbsolute));
                    vWindowsMediaPlayer.Play();

                    //Allow sound to finish
                    await Task.Delay(1000);
                    return;
                }

                //Save screenshot to file
                string fileName = DateTime.Now.ToString("HH.mm.ss.ffff") + " (" + DateTime.Now.ToShortDateString() + ")";
                if (vCaptureDetails.HDREnabled)
                {
                    if (vCaptureSettings.HDRtoSDR)
                    {
                        fileName += " (HDRtoSDR)";
                    }
                    else
                    {
                        fileName += " (HDR)";
                    }
                }
                else
                {
                    fileName += " (SDR)";
                }

                bool screenshotExport = false;
                if (vSettingImageFormat == "bmp")
                {
                    screenshotExport = CaptureImport.CaptureSaveFileBmp(bitmapIntPtr, "Screenshots\\Screenshot " + fileName + ".bmp");
                    Debug.WriteLine("Screenshot bmp export succeeded: " + screenshotExport);
                }
                else if (vSettingImageFormat == "jpg")
                {
                    screenshotExport = CaptureImport.CaptureSaveFileJpg(bitmapIntPtr, "Screenshots\\Screenshot " + fileName + ".jpg", vSettingImageQuality);
                    Debug.WriteLine("Screenshot jpg export succeeded: " + screenshotExport);
                }
                else if (vSettingImageFormat == "png")
                {
                    screenshotExport = CaptureImport.CaptureSaveFilePng(bitmapIntPtr, "Screenshots\\Screenshot " + fileName + ".png");
                    Debug.WriteLine("Screenshot png export succeeded: " + screenshotExport);
                }
                else if (vSettingImageFormat == "jxr")
                {
                    screenshotExport = CaptureImport.CaptureSaveFileJxr(bitmapIntPtr, "Screenshots\\Screenshot " + fileName + ".jxr");
                    Debug.WriteLine("Screenshot jxr export succeeded: " + screenshotExport);
                }

                //Play capture sound
                if (screenshotExport)
                {
                    vWindowsMediaPlayer.Volume = 1.0;
                    vWindowsMediaPlayer.Open(new Uri("Resources\\Screenshot.mp3", UriKind.RelativeOrAbsolute));
                    vWindowsMediaPlayer.Play();
                }
                else
                {
                    vWindowsMediaPlayer.Volume = 1.0;
                    vWindowsMediaPlayer.Open(new Uri("Resources\\ScreenshotFail.mp3", UriKind.RelativeOrAbsolute));
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

                //Exit application
                Application.Current.Shutdown();
            }
        }

        //Initialize Screen Capture
        private async Task<bool> InitializeScreenCapture(int delayTime)
        {
            try
            {
                Debug.WriteLine("Initializing screen capture: " + DateTime.Now);

                //Initialize screen capture
                bool captureInitialized = CaptureImport.CaptureInitialize(vCaptureSettings, out vCaptureDetails);

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
                return captureInitialized;
            }
            catch (Exception ex)
            {
                Debug.WriteLine("Failed to initialize screen capture: " + ex.Message);
                return false;
            }
            finally
            {
                await Task.Delay(delayTime);
            }
        }
    }
}