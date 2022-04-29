using System;
using System.Diagnostics;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Interop;
using System.Windows.Media;
using static ScreenCapture.AppImport;

namespace ScreenCapture
{
    public partial class MainWindow : Window
    {
        //Window Variables
        private IntPtr vInteropWindowHandle = IntPtr.Zero;

        //Player Variables
        private static MediaPlayer vWindowsMediaPlayer = new MediaPlayer();

        //Screen Capture Variables
        public static CaptureDetails vCaptureDetails;
        public static CaptureSettings vCaptureSettings;

        //Initialize window
        public MainWindow()
        {
            InitializeComponent();
        }

        //Initialize Screen Capture
        private async Task<bool> InitializeScreenCapture(int delayTime)
        {
            try
            {
                Debug.WriteLine("Initializing screen capture: " + DateTime.Now);

                //Set capture settings
                vCaptureSettings = new CaptureSettings
                {
                    MonitorId = 0,
                    MaxPixelDimension = 1000,
                    HDRtoSDR = true,
                    HDRBrightness = (float)slider_HDRBrightness.Value,
                    Saturation = (float)slider_Saturation.Value,
                    RedChannel = (float)slider_RedChannel.Value,
                    GreenChannel = (float)slider_GreenChannel.Value,
                    BlueChannel = (float)slider_BlueChannel.Value,
                    Brightness = (float)slider_Brightness.Value,
                    Contrast = (float)slider_Contrast.Value,
                    Gamma = (float)slider_Gamma.Value,
                };

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

                //Update interface details
                Debug.WriteLine(captureDetails);
                textblock_CaptureDetails.Text = captureDetails;
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

        //Handle window initialized event
        protected override async void OnSourceInitialized(EventArgs e)
        {
            try
            {
                //Get interop window handle
                vInteropWindowHandle = new WindowInteropHelper(this).EnsureHandle();

                //Register Hotkeys and Filtermessage
                ComponentDispatcher.ThreadFilterMessage += ComponentDispatcher_ThreadFilterMessage;
                RegisterHotKey(vInteropWindowHandle, HOTKEY_ID, 0x0001, 0x7B); //Alt+F12

                //Initialize screen capture
                await InitializeScreenCapture(500);

                //Loop and capture screen
                while (true)
                {
                    IntPtr bitmapIntPtr = IntPtr.Zero;
                    try
                    {
                        //Check window state
                        if (this.WindowState == WindowState.Minimized)
                        {
                            Debug.WriteLine("Minimized skipping preview update.");
                            continue;
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
                            Debug.WriteLine("Screenshot is corrupted, restarting capture.");
                            await InitializeScreenCapture(500);
                            continue;
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

                        if ((bool)checkbox_SaveBmp.IsChecked)
                        {
                            bool screenshotExport = CaptureImport.CaptureSaveFileBmp(bitmapIntPtr, "Screenshots\\Screenshot " + fileName + ".bmp");
                            Debug.WriteLine("Screenshot bmp export succeeded: " + screenshotExport);
                        }

                        if ((bool)checkbox_SaveJpg.IsChecked)
                        {
                            bool screenshotExport = CaptureImport.CaptureSaveFileJpg(bitmapIntPtr, "Screenshots\\Screenshot " + fileName + ".jpg", 10);
                            Debug.WriteLine("Screenshot jpg export succeeded: " + screenshotExport);
                        }

                        if ((bool)checkbox_SavePng.IsChecked)
                        {
                            bool screenshotExport = CaptureImport.CaptureSaveFilePng(bitmapIntPtr, "Screenshots\\Screenshot " + fileName + ".png");
                            Debug.WriteLine("Screenshot png export succeeded: " + screenshotExport);
                        }

                        if ((bool)checkbox_SaveJxr.IsChecked)
                        {
                            bool screenshotExport = CaptureImport.CaptureSaveFileJxr(bitmapIntPtr, "Screenshots\\Screenshot " + fileName + ".jxr");
                            Debug.WriteLine("Screenshot jxr export succeeded: " + screenshotExport);
                        }

                        //Update screen capture preview
                        image_DebugPreview.Source = CaptureBitmap.BitmapIntPtrToBitmapSource(bitmapIntPtr, vCaptureDetails, vCaptureSettings);
                    }
                    catch (Exception ex)
                    {
                        Debug.WriteLine("Screen capture loop failed: " + ex.Message);
                    }
                    finally
                    {
                        //Clear screen capture resources
                        if (bitmapIntPtr != IntPtr.Zero)
                        {
                            CaptureImport.CaptureFreeMemory(bitmapIntPtr);
                        }

                        //Delay next screen capture
                        await Task.Delay(500);
                    }
                }
            }
            catch { }
        }

        private void ComponentDispatcher_ThreadFilterMessage(ref MSG msg, ref bool handled)
        {
            try
            {
                if (handled) { return; }
                if (msg.message == WM_HOTKEY)
                {
                    int usedVirtualKey = ((int)msg.lParam >> 16) & 0xFFFF;
                    if (usedVirtualKey == 0x7B)
                    {
                        TakeScreenshotAndSave();
                        handled = true;
                    }
                }
            }
            catch { }
        }

        private static void TakeScreenshotAndSave()
        {
            try
            {
                //Capture screenshot
                IntPtr bitmapIntPtr = CaptureImport.CaptureScreenshot();

                //Screenshot filename
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

                //Save screenshot file
                bool screenshotExport = CaptureImport.CaptureSaveFilePng(bitmapIntPtr, "Screenshots\\Screenshot " + fileName + ".png");
                Debug.WriteLine("Screenshot png export succeeded: " + screenshotExport);

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
            }
            catch { }
        }

        private void Button_OpenDir_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                Process.Start("Screenshots");
            }
            catch { }
        }

        private void slider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            try
            {
                //Update slider text
                text_HDRBrightness.Text = text_HDRBrightness.Tag + " " + slider_HDRBrightness.Value.ToString("0.0000");
                text_Saturation.Text = text_Saturation.Tag + " " + slider_Saturation.Value.ToString("0.0000");
                text_RedChannel.Text = text_RedChannel.Tag + " " + slider_RedChannel.Value.ToString("0.0000");
                text_GreenChannel.Text = text_GreenChannel.Tag + " " + slider_GreenChannel.Value.ToString("0.0000");
                text_BlueChannel.Text = text_BlueChannel.Tag + " " + slider_BlueChannel.Value.ToString("0.0000");
                text_Brightness.Text = text_Brightness.Tag + " " + slider_Brightness.Value.ToString("0.0000");
                text_Contrast.Text = text_Contrast.Tag + " " + slider_Contrast.Value.ToString("0.0000");
                text_Gamma.Text = text_Gamma.Tag + " " + slider_Gamma.Value.ToString("0.0000");

                //Set capture settings
                vCaptureSettings = new CaptureSettings
                {
                    MonitorId = 0,
                    MaxPixelDimension = 1000,
                    HDRtoSDR = true,
                    HDRBrightness = (float)slider_HDRBrightness.Value,
                    Saturation = (float)slider_Saturation.Value,
                    RedChannel = (float)slider_RedChannel.Value,
                    GreenChannel = (float)slider_GreenChannel.Value,
                    BlueChannel = (float)slider_BlueChannel.Value,
                    Brightness = (float)slider_Brightness.Value,
                    Contrast = (float)slider_Contrast.Value,
                    Gamma = (float)slider_Gamma.Value,
                };

                bool settingsUpdated = CaptureImport.CaptureUpdateSettings(vCaptureSettings);
                Debug.WriteLine("Capture settings updated: " + settingsUpdated);
            }
            catch { }
        }
    }
}