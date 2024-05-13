using ArnoldVinkCode;
using ScreenCaptureImport;
using System;
using System.Diagnostics;
using System.Threading.Tasks;
using System.Windows;
using static ScreenCaptureImport.CaptureImport;

namespace ScreenCapture
{
    public partial class MainWindow : Window
    {
        //Screen Capture Variables
        public static CaptureDetails vCaptureDetails;
        public static CaptureSettings vCaptureSettings;

        //Initialize window
        public MainWindow()
        {
            InitializeComponent();
        }

        //Screen capture events
        public static CaptureEvent CaptureEventDeviceChangeDetected = null;
        public void CaptureEventDeviceChangeDetectedVoid()
        {
            try
            {
                Debug.WriteLine("Device change event triggered, restarting capture.");
                InitializeScreenCapture();
            }
            catch { }
        }

        //Initialize screen capture
        private CaptureStatus InitializeScreenCapture()
        {
            CaptureStatus captureInitialized = CaptureStatus.Failed;
            try
            {
                AVActions.DispatcherInvoke(delegate
                {
                    //Register capture events
                    CaptureEventDeviceChangeDetected = new CaptureEvent(CaptureEventDeviceChangeDetectedVoid);
                    CaptureImport.CaptureEventDeviceChangeDetected(CaptureEventDeviceChangeDetected);

                    //Set capture settings
                    vCaptureSettings = new CaptureSettings
                    {
                        MonitorId = 0,
                        MaxPixelDimension = 1000,
                        DrawMouseCursor = true,
                        HDRtoSDR = true,
                        HDRPaperWhite = (float)slider_HDRPaperWhite.Value,
                        HDRMaximumNits = (float)slider_HDRMaximumNits.Value,
                        Saturation = (float)slider_Saturation.Value,
                        RedChannel = (float)slider_RedChannel.Value,
                        GreenChannel = (float)slider_GreenChannel.Value,
                        BlueChannel = (float)slider_BlueChannel.Value,
                        Brightness = (float)slider_Brightness.Value,
                        Contrast = (float)slider_Contrast.Value,
                        Gamma = (float)slider_Gamma.Value,
                        Blur = (float)slider_Blur.Value
                    };

                    //Initialize screen capture
                    captureInitialized = CaptureImport.CaptureInitialize(vCaptureSettings, true);
                    if (captureInitialized == CaptureStatus.Initialized)
                    {
                        //Get capture details
                        vCaptureDetails = CaptureImport.CaptureGetDetails();

                        //Set capture details string
                        string captureDetailsString = "Width: " + vCaptureDetails.OutputWidth + " (" + vCaptureDetails.OriginalWidth + ")";
                        captureDetailsString += "\nHeight: " + vCaptureDetails.OutputHeight + " (" + vCaptureDetails.OriginalHeight + ")";
                        captureDetailsString += "\nRefreshRate: " + vCaptureDetails.RefreshRate;
                        captureDetailsString += "\nPixelByteSize: " + vCaptureDetails.PixelByteSize;
                        captureDetailsString += "\nWidthByteSize: " + vCaptureDetails.WidthByteSize;
                        captureDetailsString += "\nTotalByteSize: " + vCaptureDetails.TotalByteSize;
                        captureDetailsString += "\nHDREnabled: " + vCaptureDetails.HDREnabled;
                        captureDetailsString += "\nHDRtoSDR: " + vCaptureDetails.HDRtoSDR;
                        captureDetailsString += "\nSDRWhiteLevel: " + vCaptureDetails.SDRWhiteLevel;

                        //Update interface details
                        textblock_CaptureDetails.Text = captureDetailsString;
                    }
                    else if (captureInitialized == CaptureStatus.Busy)
                    {
                        //Update interface details
                        textblock_CaptureDetails.Text = "Already initializing capture.";
                    }
                    else
                    {
                        //Update interface details
                        textblock_CaptureDetails.Text = "Failed to initialize capture.";
                    }
                });

                //Return result
                return captureInitialized;
            }
            catch (Exception ex)
            {
                Debug.WriteLine("Failed to initialize screen capture: " + ex.Message);
                return captureInitialized;
            }
        }

        //Handle window initialized event
        protected override async void OnSourceInitialized(EventArgs e)
        {
            try
            {
                //Initialize screen capture
                InitializeScreenCapture();

                //Loop and capture screen
                while (true)
                {
                    try
                    {
                        //Check window state
                        if (this.WindowState == WindowState.Minimized)
                        {
                            Debug.WriteLine("Minimized skipping preview update.");
                            continue;
                        }

                        //Capture screen bytes
                        IntPtr bitmapIntPtr = IntPtr.Zero;
                        try
                        {
                            bitmapIntPtr = CaptureImport.CaptureScreenBytes();
                        }
                        catch { }

                        //Check screen bytes
                        if (bitmapIntPtr == IntPtr.Zero)
                        {
                            continue;
                        }

                        //Update screen capture preview
                        image_DebugPreview.Source = CaptureBitmap.BitmapIntPtrToBitmapSource(bitmapIntPtr, vCaptureDetails);
                    }
                    catch (Exception ex)
                    {
                        Debug.WriteLine("Screen capture loop failed: " + ex.Message);
                    }
                    finally
                    {
                        //Delay next screen capture
                        await Task.Delay(50);
                    }
                }
            }
            catch { }
        }

        private void slider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            try
            {
                //Update slider text
                text_HDRPaperWhite.Text = text_HDRPaperWhite.Tag + " " + slider_HDRPaperWhite.Value.ToString("0.0000");
                text_HDRMaximumNits.Text = text_HDRMaximumNits.Tag + " " + slider_HDRMaximumNits.Value.ToString("0.0000");
                text_Saturation.Text = text_Saturation.Tag + " " + slider_Saturation.Value.ToString("0.0000");
                text_RedChannel.Text = text_RedChannel.Tag + " " + slider_RedChannel.Value.ToString("0.0000");
                text_GreenChannel.Text = text_GreenChannel.Tag + " " + slider_GreenChannel.Value.ToString("0.0000");
                text_BlueChannel.Text = text_BlueChannel.Tag + " " + slider_BlueChannel.Value.ToString("0.0000");
                text_Brightness.Text = text_Brightness.Tag + " " + slider_Brightness.Value.ToString("0.0000");
                text_Contrast.Text = text_Contrast.Tag + " " + slider_Contrast.Value.ToString("0.0000");
                text_Gamma.Text = text_Gamma.Tag + " " + slider_Gamma.Value.ToString("0.0000");
                text_Blur.Text = text_Blur.Tag + " " + slider_Blur.Value.ToString("0.0000");

                //Set capture settings
                vCaptureSettings = new CaptureSettings
                {
                    MonitorId = 0,
                    MaxPixelDimension = 1000,
                    DrawMouseCursor = true,
                    HDRtoSDR = true,
                    HDRPaperWhite = (float)slider_HDRPaperWhite.Value,
                    HDRMaximumNits = (float)slider_HDRMaximumNits.Value,
                    Saturation = (float)slider_Saturation.Value,
                    RedChannel = (float)slider_RedChannel.Value,
                    GreenChannel = (float)slider_GreenChannel.Value,
                    BlueChannel = (float)slider_BlueChannel.Value,
                    Brightness = (float)slider_Brightness.Value,
                    Contrast = (float)slider_Contrast.Value,
                    Gamma = (float)slider_Gamma.Value,
                    Blur = (float)slider_Blur.Value,
                };

                bool settingsUpdated = CaptureImport.CaptureUpdateSettings(vCaptureSettings);
                Debug.WriteLine("Capture settings updated: " + settingsUpdated);
            }
            catch { }
        }

        private void button_Restart_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                //Initialize screen capture
                CaptureStatus captureResult = InitializeScreenCapture();
                Debug.WriteLine("Capture restart: " + captureResult);
            }
            catch { }
        }

        private void button_Stop_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                //Reset screen capture
                bool captureResult = CaptureImport.CaptureReset();
                Debug.WriteLine("Capture stopped: " + captureResult);
            }
            catch { }
        }
    }
}