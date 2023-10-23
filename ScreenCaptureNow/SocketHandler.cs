using ArnoldVinkCode;
using System.Diagnostics;

namespace ScreenCapture
{
    partial class SocketServer
    {
        //Handle received pipe string
        public static void ReceivedPipesHandler(string receivedString)
        {
            try
            {
                void TaskAction()
                {
                    try
                    {
                        Debug.WriteLine("Received pipe string: " + receivedString);
                        if (receivedString == "-videostop")
                        {
                            AppClose.Application_Exit();
                        }
                    }
                    catch { }
                }
                AVActions.TaskStartBackground(TaskAction);
            }
            catch { }
        }
    }
}