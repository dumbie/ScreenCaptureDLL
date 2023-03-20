using System;
using System.Windows;
using static ArnoldVinkCode.AVSettings;
using static ScreenCapture.AppVariables;

namespace ScreenCapture
{
    public partial class WindowMain : Window
    {
        //Window Initialize
        public WindowMain() { InitializeComponent(); }

        //Window Initialized
        protected override void OnSourceInitialized(EventArgs e)
        {
            try
            {
                //Update first launch setting
                SettingSave(vConfiguration, "AppFirstLaunch", "False");

                //Make sure the correct window style is set
                StateChanged += CheckWindowStateAndStyle;
            }
            catch { }
        }

        //Make sure the correct window style is set
        void CheckWindowStateAndStyle(object sender, EventArgs e)
        {
            try
            {
                if (WindowState == WindowState.Minimized) { Application_ShowHideWindow(); }
            }
            catch { }
        }
    }
}