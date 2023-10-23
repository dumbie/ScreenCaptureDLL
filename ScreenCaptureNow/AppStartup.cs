using ArnoldVinkCode;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Threading.Tasks;

namespace ScreenCapture
{
    public partial class AppStartup
    {
        //Startup application
        public static async Task Application_Startup(string[] launchArgs)
        {
            try
            {
                Debug.WriteLine("Starting application.");

                //Set the working directory to executable directory
                Directory.SetCurrentDirectory(Path.GetDirectoryName(Assembly.GetEntryAssembly().Location));

                //Check launch arguments
                await CheckLaunchAction(launchArgs);
            }
            catch { }
        }

        //Check launch arguments
        public static async Task CheckLaunchAction(string[] launchArgs)
        {
            try
            {
                int launchAction = 0;
                if (launchArgs != null && launchArgs.Any())
                {
                    foreach (string launchArgument in launchArgs)
                    {
                        try
                        {
                            //Convert launch argument to lower
                            string launchArgumentLower = launchArgument.ToLower();
                            if (launchArgumentLower.StartsWith("-image"))
                            {
                                launchAction = 1;
                            }
                            else if (launchArgumentLower.StartsWith("-video"))
                            {
                                launchAction = 2;
                            }
                        }
                        catch { }
                    }
                }

                //Check launch action
                if (launchAction == 1)
                {
                    //Capture image to file
                    await ScreenCaptureCode.CaptureImageToFile();
                }
                else if (launchAction == 2)
                {
                    //Capture video to file
                    await ScreenCaptureCode.StartCaptureVideoToFile();
                }
                else
                {
                    List<string> messageAnswers = new List<string>();
                    messageAnswers.Add("Ok");
                    await new AVMessageBox().Popup(null, "Screen Capture Now", "Please use the Screen Capture Tool to start screen captures.", messageAnswers);

                    AppClose.Application_Exit();
                }
            }
            catch { }
        }
    }
}