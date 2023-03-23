using ArnoldVinkCode;
using System.Configuration;
using System.Windows.Media;
using static ArnoldVinkCode.AVSettings;

namespace ScreenCapture
{
    public partial class AppVariables
    {
        //Application Variables
        public static Configuration vConfiguration = SettingLoadConfig("ScreenCaptureTool.exe.csettings");

        //Setting Variables
        public static bool vComboboxSaveEnabled = true;

        //Player Variables
        public static MediaPlayer vWindowsMediaPlayer = new MediaPlayer();

        //Sockets Variables
        public static ArnoldVinkSockets vArnoldVinkSockets = null;
    }
}