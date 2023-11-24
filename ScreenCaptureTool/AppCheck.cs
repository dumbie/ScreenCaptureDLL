using System.Diagnostics;
using System.IO;
using System.Reflection;
using static ArnoldVinkCode.AVFirewall;

namespace ScreenCapture
{
    public partial class AppCheck
    {
        public static void Startup_Check()
        {
            try
            {
                Debug.WriteLine("Checking application status.");

                //Get path to application executable file
                string appFilePath = Assembly.GetEntryAssembly().Location;

                //Set the working directory to executable directory
                try
                {
                    Directory.SetCurrentDirectory(Path.GetDirectoryName(appFilePath));
                }
                catch { }

                //Allow application in firewall
                try
                {
                    Firewall_ExecutableAllow("Screen Capture Tool", appFilePath, true);
                }
                catch { }
            }
            catch { }
        }
    }
}