using System.Configuration;
using System.Windows.Media;
using static ArnoldVinkCode.AVSettings;

namespace ScreenCapture
{
    public partial class AppVariables
    {
        //Application Variables
        public static Configuration vConfiguration = SettingLoadConfig("ScreenCaptureTool.exe.csettings");

        //Player Variables
        public static MediaPlayer vWindowsMediaPlayer = new MediaPlayer();
    }
}