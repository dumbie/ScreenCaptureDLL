using System.IO;
using System.Reflection;
using System.Windows;

namespace ScreenCapture
{
    public partial class App : Application
    {
        protected override async void OnStartup(StartupEventArgs e)
        {
            try
            {
                //Set the working directory to executable directory
                Directory.SetCurrentDirectory(Path.GetDirectoryName(Assembly.GetEntryAssembly().Location));

                //Capture screenshot to file
                await CaptureScreen.CaptureScreenToFile(e.Args);
            }
            catch { }
        }
    }
}