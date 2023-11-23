using ArnoldVinkCode;
using ScreenCaptureNow;
using System.Configuration;
using static ArnoldVinkCode.AVSettings;

namespace ScreenCapture
{
    public partial class AppVariables
    {
        //Setting Variables
        public static Configuration vConfigurationTool = SettingLoadConfig("ScreenCaptureTool.exe.csettings");

        //Pipes Variables
        public static ArnoldVinkPipes vArnoldVinkPipes = null;

        //Capture Variables
        public static string vCaptureFileName = string.Empty;

        //Window Variables
        public static WindowOverlay vWindowOverlay = new WindowOverlay();
    }
}