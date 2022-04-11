using System;
using System.Diagnostics;
using System.Drawing;
using System.Threading.Tasks;
using System.Windows;

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
        public static int vCaptureTotalByteSize = 0;
        public static bool vCaptureHDREnabled = false;

        //Initialize Screen Capture
        private static async Task<bool> InitializeScreenCapture(int delayTime)
        {
            try
            {
                Debug.WriteLine("Initializing screen capture: " + DateTime.Now);
                bool initialized = AppImport.CaptureInitialize(0, 800, false, out vCaptureWidth, out vCaptureHeight, out vCaptureTotalByteSize, out vCaptureHDREnabled);
                Debug.WriteLine("ScreenOutputWidth: " + vCaptureWidth);
                Debug.WriteLine("ScreenOutputHeight: " + vCaptureHeight);
                Debug.WriteLine("ScreenOutputSize: " + vCaptureTotalByteSize);
                Debug.WriteLine("HDR enabled: " + vCaptureHDREnabled);
                return initialized;
            }
            catch
            {
                Debug.WriteLine("Failed to initialize screen capture.");
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
                await InitializeScreenCapture(500);
                while (true)
                {
                    IntPtr BitmapIntPtr = IntPtr.Zero;
                    Bitmap BitmapImage = null;
                    try
                    {
                        //Capture screenshot
                        try
                        {
                            BitmapIntPtr = AppImport.CaptureScreenshot();
                        }
                        catch { }

                        //Check screenshot
                        if (BitmapIntPtr == IntPtr.Zero)
                        {
                            Debug.WriteLine("Screenshot is corrupted, restarting capture.");
                            await InitializeScreenCapture(500);
                            continue;
                        }

                        unsafe
                        {
                            //Convert pointer to bytes
                            byte* BitmapData = (byte*)BitmapIntPtr;

                            //Convert data to bitmap
                            BitmapImage = BitmapProcessing.BitmapFromData(BitmapData, vCaptureWidth, vCaptureHeight, vCaptureTotalByteSize, false);
                        }

                        //Save screenshot to file
                        string fileName = DateTime.Now.ToString("HH.mm.ss.ffff (MM-dd-yyyy)");
                        if (vCaptureHDREnabled)
                        {
                            fileName += " (HDR)";
                        }
                        else
                        {
                            fileName += " (SDR)";
                        }
                        //bool screenshotExport = AppImport.CaptureSaveFileBmp(BitmapIntPtr, "Screenshots\\Screenshot " + fileName + ".bmp");
                        //bool screenshotExport = AppImport.CaptureSaveFileJpg(BitmapIntPtr, "Screenshots\\Screenshot " + fileName + ".jpg", 100);
                        //bool screenshotExport = AppImport.CaptureSaveFilePng(BitmapIntPtr, "Screenshots\\Screenshot " + fileName + ".png");
                        //bool screenshotExport = AppImport.CaptureSaveFileJxr(BitmapIntPtr, "Screenshots\\Screenshot " + fileName + ".jxr");
                        //Debug.WriteLine("Screenshot export succeeded: " + screenshotExport);

                        //Update screen capture preview
                        image_DebugPreview.Source = BitmapProcessing.BitmapToBitmapImage(BitmapImage);
                    }
                    catch (Exception ex)
                    {
                        Debug.WriteLine("Screen capture loop failed: " + ex.Message);
                    }
                    finally
                    {
                        //Clear screen capture resources
                        if (BitmapIntPtr != IntPtr.Zero)
                        {
                            AppImport.CaptureFreeMemory(BitmapIntPtr);
                        }
                        if (BitmapImage != null)
                        {
                            BitmapImage.Dispose();
                        }

                        await Task.Delay(500);
                    }
                }
            }
            catch { }
        }
    }
}