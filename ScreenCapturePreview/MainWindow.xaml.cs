using System;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Imaging;

namespace ScreenCapturePreview
{
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        //Screen Capture Variables
        public static int vCaptureWidth = 0;
        public static int vCaptureHeight = 0;
        public static int vCaptureWidthByteSize = 0;
        public static int vCaptureTotalByteSize = 0;
        public static bool vCaptureHDREnabled = false;
        public static bool vCaptureHDRtoSDR = true;

        //Initialize Screen Capture
        private static async Task<bool> InitializeScreenCapture(int delayTime)
        {
            try
            {
                Debug.WriteLine("Initializing screen capture: " + DateTime.Now);
                bool initialized = AppImport.CaptureInitialize(0, 800, vCaptureHDRtoSDR, out vCaptureWidth, out vCaptureHeight, out vCaptureWidthByteSize, out vCaptureTotalByteSize, out vCaptureHDREnabled);
                Debug.WriteLine("CaptureWidth: " + vCaptureWidth);
                Debug.WriteLine("CaptureHeight: " + vCaptureHeight);
                Debug.WriteLine("CaptureWidthByteSize: " + vCaptureWidthByteSize);
                Debug.WriteLine("CaptureTotalByteSize: " + vCaptureTotalByteSize);
                Debug.WriteLine("CaptureHDREnabled: " + vCaptureHDREnabled);
                Debug.WriteLine("CaptureHDRtoSDR: " + vCaptureHDRtoSDR);
                return initialized;
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
                byte[] bitmapDataArray = new byte[vCaptureTotalByteSize];
                Marshal.Copy(bitmapIntPtr, bitmapDataArray, 0, vCaptureTotalByteSize);

                PixelFormat bitmapPixelFormat = PixelFormats.Bgra32;
                if (vCaptureHDREnabled && !vCaptureHDRtoSDR)
                {
                    bitmapPixelFormat = PixelFormats.Rgba64;
                }

                return BitmapSource.Create(vCaptureWidth, vCaptureHeight, 96, 96, bitmapPixelFormat, null, bitmapDataArray, vCaptureWidthByteSize);
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
                await InitializeScreenCapture(500);
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
                        if (vCaptureHDREnabled)
                        {
                            if (vCaptureHDRtoSDR)
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

                        await Task.Delay(500);
                    }
                }
            }
            catch { }
        }
    }
}