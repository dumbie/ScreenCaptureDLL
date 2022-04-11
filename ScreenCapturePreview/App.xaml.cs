using System.IO;
using System.Reflection;
using System.Windows;

namespace ScreenCapturePreview
{
    public partial class App : Application
    {
        protected override void OnStartup(StartupEventArgs e)
        {
            try
            {
                //Set the working directory to executable directory
                Directory.SetCurrentDirectory(Path.GetDirectoryName(Assembly.GetEntryAssembly().Location));

                //Create screenshots export directory
                if (!Directory.Exists("Screenshots"))
                {
                    Directory.CreateDirectory("Screenshots");
                }
            }
            catch { }
        }
    }
}