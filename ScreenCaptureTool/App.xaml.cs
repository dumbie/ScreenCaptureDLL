using System.Windows;
using static ArnoldVinkCode.AVInteropDll;
using static ScreenCapture.AppCheck;

namespace ScreenCapture
{
    public partial class App : Application
    {
        //Application Windows
        public static WindowMain vWindowMain = new WindowMain();

        //Application Startup
        protected override async void OnStartup(StartupEventArgs e)
        {
            try
            {
                //Application startup checks
                StartupCheck("Screen Capture Tool", ProcessPriority.Normal);

                //Run application startup code
                await vWindowMain.Startup();
            }
            catch { }
        }
    }
}