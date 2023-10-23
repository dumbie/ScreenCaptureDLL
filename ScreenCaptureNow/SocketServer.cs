using ArnoldVinkCode;
using static ScreenCapture.AppVariables;

namespace ScreenCapture
{
    public partial class SocketServer
    {
        //Enable the pipes server
        public static void EnablePipesServer()
        {
            try
            {
                vArnoldVinkPipes = new ArnoldVinkPipes("ScreenCapturer");
                vArnoldVinkPipes.PipeServerEnable();
                vArnoldVinkPipes.EventStringReceived += ReceivedPipesHandler;
            }
            catch { }
        }
    }
}