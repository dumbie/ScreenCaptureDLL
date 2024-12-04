using ArnoldVinkCode;
using ScreenCaptureImport;
using System.Collections.Generic;
using System.Configuration;
using static ArnoldVinkCode.AVClasses;
using static ArnoldVinkCode.AVJsonFunctions;
using static ArnoldVinkCode.AVSettings;

namespace ScreenCapture
{
    public partial class AppVariables
    {
        //Application Variables
        public static Configuration vConfigurationScreenCaptureTool = SettingLoadConfig("ScreenCaptureTool.exe.csettings");
        public static Configuration vConfigurationCtrlUI = SettingLoadConfig("CtrlUI.exe.csettings");

        //Setting Variables
        public static bool vComboboxSaveEnabled = true;

        //Sockets Variables
        public static ArnoldVinkSockets vArnoldVinkSockets = null;

        //Pipes Variables
        public static ArnoldVinkPipes vArnoldVinkPipes = null;

        //Capture Variables
        public static CaptureDetails vCaptureDetails;
        public static string vCaptureFileName = string.Empty;

        //Application Windows
        public static WindowMain vWindowMain = new WindowMain();
        public static WindowOverlay vWindowOverlay = new WindowOverlay();

        //Application Lists
        public static List<ShortcutTriggerKeyboard> vShortcutTriggers = JsonLoadFile<List<ShortcutTriggerKeyboard>>(@"Profiles\User\CaptureShortcutKeyboard.json");
    }
}