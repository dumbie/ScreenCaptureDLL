using System.Windows;
using static ArnoldVinkCode.AVInteropDll;
using static ArnoldVinkCode.AVStartup;

namespace ScreenCapture
{
    public partial class App : Application
    {
        protected override async void OnStartup(StartupEventArgs e)
        {
            try
            {
                //Setup application defaults
                SetupDefaults(ProcessPriority.High, false);

                //Run application startup code
                await AppStartup.Startup(e.Args);
            }
            catch { }
        }
    }
}