using ArnoldVinkCode;
using System;
using System.Windows;
using System.Windows.Interop;
using System.Windows.Threading;
using static ArnoldVinkCode.AVInteropDll;
using static ArnoldVinkCode.AVWindowFunctions;

namespace ScreenCaptureNow
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
                SetWindowDisplayAffinity(vInteropWindowHandle, DisplayAffinity.ExcludeFromCapture);

                //Start timing update timer
                vRecordingTime = 0;
                AVFunctions.TimerRenew(ref vDispatcherTimerDelay);
                vDispatcherTimerDelay.Interval = TimeSpan.FromMilliseconds(1000);
                vDispatcherTimerDelay.Tick += VDispatcherTimerDelay_Tick;
                vDispatcherTimerDelay.Start();
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