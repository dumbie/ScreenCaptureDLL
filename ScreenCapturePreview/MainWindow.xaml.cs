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
                    MaxPixelDimension = 800,
                    HDRtoSDR = true,
                    HDRBrightness = 70.0F
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
                captureDetails += "\nHDRBrightness: " + vCaptureSettings.HDRBrightness;
                captureDetails += "\nSDRWhiteLevel: " + vCaptureDetails.SDRWhiteLevel;

                //Update interface details
                Debug.WriteLine(captureDetails);
                textblock_CaptureDetails.Text = "Capture details:\n" + captureDetails;
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
                    bitmapPixelFormat = PixelFormats.Rgba64;
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
                        //bool screenshotExport = AppImport.CaptureSaveFileBmp(bitmapIntPtr, "Screenshots\\Screenshot " + fileName + ".bmp");
                        //bool screenshotExport = AppImport.CaptureSaveFileJpg(bitmapIntPtr, "Screenshots\\Screenshot " + fileName + ".jpg", 75);
                        //bool screenshotExport = AppImport.CaptureSaveFilePng(bitmapIntPtr, "Screenshots\\Screenshot " + fileName + ".png");
                        //bool screenshotExport = AppImport.CaptureSaveFileJxr(bitmapIntPtr, "Screenshots\\Screenshot " + fileName + ".jxr");
                        //Debug.WriteLine("Screenshot export succeeded: " + screenshotExport);

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
    }
}