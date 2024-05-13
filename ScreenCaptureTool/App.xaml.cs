using System.Windows;

namespace ScreenCapture
{
    public partial class App : Application
    {
        protected override async void OnStartup(StartupEventArgs e)
        {
            try
            {
                await AppStartup.Startup(e.Args);
            }
            catch { }
        }
    }
}