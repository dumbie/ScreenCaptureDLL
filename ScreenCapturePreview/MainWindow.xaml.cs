using System;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using static ScreenCapturePreview.CaptureClasses;

namespace ScreenCapturePreview
{
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        //Screen Capture Variables
        public static CaptureDetails vCaptureDetails;
        public static CaptureSettings vCaptureSettings;

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
                    Vibrance = (float)slider_Vibrance.Value,
                    Saturate = (float)slider_Saturate.Value,
                    Temperature = (float)slider_Temperature.Value,
                    RedChannel = (float)slider_RedChannel.Value,
                    GreenChannel = (float)slider_GreenChannel.Value,
                    BlueChannel = (float)slider_BlueChannel.Value,
                    Brightness = (float)slider_Brightness.Value,
                    Contrast = (float)slider_Contrast.Value,
                    Gamma = (float)slider_Gamma.Value,
                };

                //Initialize screen capture
                bool captureInitialized = AppImport.CaptureInitialize(vCaptureSettings, out vCaptureDetails);

                //Set capture details string
                string captureDetails = "Width: " + vCaptureDetails.Width;
                captureDetails += "\nHeight: " + vCaptureDetails.Height;
                captureDetails += "\nPixelByteSize: " + vCaptureDetails.PixelByteSize;
                captureDetails += "\nWidthByteSize: " + vCaptureDetails.WidthByteSize;
                captureDetails += "\nTotalByteSize: " + vCaptureDetails.TotalByteSize;
                captureDetails += "\nHDREnabled: " + vCaptureDetails.HDREnabled;
                captureDetails += "\nHDRtoSDR: " + vCaptureSettings.HDRtoSDR;
                captureDetails += "\nSDRWhiteLevel: " + vCaptureDetails.SDRWhiteLevel;

                //Update slider text
                text_HDRBrightness.Text = text_HDRBrightness.Tag + " " + slider_HDRBrightness.Value.ToString("0.0000");
                text_Vibrance.Text = text_Vibrance.Tag + " " + slider_Vibrance.Value.ToString("0.0000");
                text_Saturate.Text = text_Saturate.Tag + " " + slider_Saturate.Value.ToString("0.0000");
                text_Temperature.Text = text_Temperature.Tag + " " + slider_Temperature.Value.ToString("0.0000");
                text_RedChannel.Text = text_RedChannel.Tag + " " + slider_RedChannel.Value.ToString("0.0000");
                text_GreenChannel.Text = text_GreenChannel.Tag + " " + slider_GreenChannel.Value.ToString("0.0000");
                text_BlueChannel.Text = text_BlueChannel.Tag + " " + slider_BlueChannel.Value.ToString("0.0000");
                text_Brightness.Text = text_Brightness.Tag + " " + slider_Brightness.Value.ToString("0.0000");
                text_Contrast.Text = text_Contrast.Tag + " " + slider_Contrast.Value.ToString("0.0000");
                text_Gamma.Text = text_Gamma.Tag + " " + slider_Gamma.Value.ToString("0.0000");

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

        //Convert bitmapdata to bitmapsource
        private BitmapSource BitmapDataToBitmapSource(IntPtr bitmapIntPtr)
        {
            try
            {
                byte[] bitmapDataArray = new byte[vCaptureDetails.TotalByteSize];
                Marshal.Copy(bitmapIntPtr, bitmapDataArray, 0, vCaptureDetails.TotalByteSize);

                PixelFormat bitmapPixelFormat = PixelFormats.Bgra32;
                if (vCaptureDetails.HDREnabled && !vCaptureSettings.HDRtoSDR)
                {
                    bitmapPixelFormat = PixelFormats.Rgba64; //Fix Rgba64Half support missing 
                }

                return BitmapSource.Create(vCaptureDetails.Width, vCaptureDetails.Height, 96, 96, bitmapPixelFormat, null, bitmapDataArray, vCaptureDetails.WidthByteSize);
            }
            catch (Exception ex)
            {
                Debug.WriteLine("Failed to convert bitmapdata to bitmapsource: " + ex.Message);
                return null;
            }
        }

        //Handle window initialized event
        protected override async void OnSourceInitialized(EventArgs e)
        {
            try
            {
                //Initialize screen capture
                await InitializeScreenCapture(500);

                //Loop and capture screen
                while (true)
                {
                    IntPtr bitmapIntPtr = IntPtr.Zero;
                    try
                    {
                        //Capture screenshot
                        try
                        {
                            bitmapIntPtr = AppImport.CaptureScreenshot();
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
                        string fileName = DateTime.Now.ToString("HH.mm.ss.ffff (MM-dd-yyyy)");
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
                            bool screenshotExport = AppImport.CaptureSaveFileBmp(bitmapIntPtr, "Screenshots\\Screenshot " + fileName + ".bmp");
                            Debug.WriteLine("Screenshot bmp export succeeded: " + screenshotExport);
                        }

                        if ((bool)checkbox_SaveJpg.IsChecked)
                        {
                            bool screenshotExport = AppImport.CaptureSaveFileJpg(bitmapIntPtr, "Screenshots\\Screenshot " + fileName + ".jpg", 10);
                            Debug.WriteLine("Screenshot jpg export succeeded: " + screenshotExport);
                        }

                        if ((bool)checkbox_SavePng.IsChecked)
                        {
                            bool screenshotExport = AppImport.CaptureSaveFilePng(bitmapIntPtr, "Screenshots\\Screenshot " + fileName + ".png");
                            Debug.WriteLine("Screenshot png export succeeded: " + screenshotExport);
                        }

                        if ((bool)checkbox_SaveJxr.IsChecked)
                        {
                            bool screenshotExport = AppImport.CaptureSaveFileJxr(bitmapIntPtr, "Screenshots\\Screenshot " + fileName + ".jxr");
                            Debug.WriteLine("Screenshot jxr export succeeded: " + screenshotExport);
                        }

                        //Update screen capture preview
                        image_DebugPreview.Source = BitmapDataToBitmapSource(bitmapIntPtr);
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
                            AppImport.CaptureFreeMemory(bitmapIntPtr);
                        }

                        //Delay next screen capture
                        await Task.Delay(500);
                    }
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

        private async void slider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            try
            {
                await InitializeScreenCapture(0);
            }
            catch { }
        }
    }
}