using ArnoldVinkCode;
using System;
using System.Windows;
using System.Windows.Interop;
using System.Windows.Threading;
using static ArnoldVinkCode.AVInteropDll;
using static ArnoldVinkCode.AVSettings;
using static ArnoldVinkCode.AVWindowFunctions;
using static ScreenCapture.AppVariables;

namespace ScreenCapture
{
    public partial class WindowOverlay : Window
    {
        //Window Variables
        private IntPtr vInteropWindowHandle = IntPtr.Zero;
        private DispatcherTimer vDispatcherTimerDelay = new DispatcherTimer();
        private int vRecordingTime = 0;

        //Window Initialize
        public WindowOverlay()
        {
            InitializeComponent();
        }

        //Window Initialized
        protected override void OnSourceInitialized(EventArgs e)
        {
            try
            {
                //Get interop window handle
                vInteropWindowHandle = new WindowInteropHelper(this).EnsureHandle();

                //Set render mode to software
                HwndSource hwndSource = HwndSource.FromHwnd(vInteropWindowHandle);
                HwndTarget hwndTarget = hwndSource.CompositionTarget;
                hwndTarget.RenderMode = RenderMode.SoftwareOnly;

                //Update window visibility
                WindowUpdateVisibility(vInteropWindowHandle, true);

                //Update window style
                WindowUpdateStyle(vInteropWindowHandle, true, true, true);

                //Set window display affinity
                SetWindowDisplayAffinity(vInteropWindowHandle, DisplayAffinityFlags.WDA_EXCLUDEFROMCAPTURE);

                //Update window position
                UpdateWindowPosition();

                //Start timing update timer
                vRecordingTime = 0;
                AVFunctions.TimerRenew(ref vDispatcherTimerDelay);
                vDispatcherTimerDelay.Interval = TimeSpan.FromMilliseconds(1000);
                vDispatcherTimerDelay.Tick += VDispatcherTimerDelay_Tick;
                vDispatcherTimerDelay.Start();
            }
            catch { }
        }

        //Update window position
        private void UpdateWindowPosition()
        {
            try
            {
                string videoOverlayPosition = SettingLoad(vConfiguration, "VideoOverlayPosition", typeof(string));
                if (videoOverlayPosition == "TopLeft")
                {
                    border_Overlay.VerticalAlignment = VerticalAlignment.Top;
                    border_Overlay.HorizontalAlignment = HorizontalAlignment.Left;
                }
                else if (videoOverlayPosition == "TopLeft")
                {
                    border_Overlay.VerticalAlignment = VerticalAlignment.Top;
                    border_Overlay.HorizontalAlignment = HorizontalAlignment.Left;
                }
                else if (videoOverlayPosition == "TopCenter")
                {
                    border_Overlay.VerticalAlignment = VerticalAlignment.Top;
                    border_Overlay.HorizontalAlignment = HorizontalAlignment.Center;
                }
                else if (videoOverlayPosition == "TopRight")
                {
                    border_Overlay.VerticalAlignment = VerticalAlignment.Top;
                    border_Overlay.HorizontalAlignment = HorizontalAlignment.Right;
                }
                else if (videoOverlayPosition == "RightCenter")
                {
                    border_Overlay.VerticalAlignment = VerticalAlignment.Center;
                    border_Overlay.HorizontalAlignment = HorizontalAlignment.Right;
                }
                else if (videoOverlayPosition == "BottomRight")
                {
                    border_Overlay.VerticalAlignment = VerticalAlignment.Bottom;
                    border_Overlay.HorizontalAlignment = HorizontalAlignment.Right;
                }
                else if (videoOverlayPosition == "BottomCenter")
                {
                    border_Overlay.VerticalAlignment = VerticalAlignment.Bottom;
                    border_Overlay.HorizontalAlignment = HorizontalAlignment.Center;
                }
                else if (videoOverlayPosition == "BottomLeft")
                {
                    border_Overlay.VerticalAlignment = VerticalAlignment.Bottom;
                    border_Overlay.HorizontalAlignment = HorizontalAlignment.Left;
                }
                else if (videoOverlayPosition == "LeftCenter")
                {
                    border_Overlay.VerticalAlignment = VerticalAlignment.Center;
                    border_Overlay.HorizontalAlignment = HorizontalAlignment.Left;
                }
            }
            catch { }
        }

        //Update recording time
        private void VDispatcherTimerDelay_Tick(object sender, EventArgs e)
        {
            try
            {
                vRecordingTime++;
                textblock_Timing.Text = AVFunctions.SecondsToHms(vRecordingTime, true, false);
            }
            catch { }
        }
    }
}