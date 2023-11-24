using System.Windows;

namespace ScreenCapture
{
    public partial class App : Application
    {
        //Application Startup
        protected override async void OnStartup(StartupEventArgs e)
        {
            try
            {
                await AppStartup.Application_Startup(e.Args);
            }
            catch { }
        }
    }
}