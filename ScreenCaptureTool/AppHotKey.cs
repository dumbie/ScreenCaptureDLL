using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using static ArnoldVinkCode.AVInputOutputClass;
using static ArnoldVinkCode.AVSettings;
using static ScreenCapture.AppVariables;

namespace ScreenCapture
{
    public partial class AppHotKey
    {
        public static async void EventHotKeyPressed(List<KeysVirtual> keysPressed)
        {
            try
            {
                //Load set shortcut image keys
                List<KeysVirtual> usedImageKeys = new List<KeysVirtual>
                {
                    (KeysVirtual)SettingLoad(vConfiguration, "ShortcutCaptureImageKey0", typeof(byte)),
                    (KeysVirtual)SettingLoad(vConfiguration, "ShortcutCaptureImageKey1", typeof(byte)),
                    (KeysVirtual)SettingLoad(vConfiguration, "ShortcutCaptureImageKey2", typeof(byte))
                };

                //Load set shortcut video keys
                List<KeysVirtual> usedVideoKeys = new List<KeysVirtual>
                {
                    (KeysVirtual)SettingLoad(vConfiguration, "ShortcutCaptureVideoKey0", typeof(byte)),
                    (KeysVirtual)SettingLoad(vConfiguration, "ShortcutCaptureVideoKey1", typeof(byte)),
                    (KeysVirtual)SettingLoad(vConfiguration, "ShortcutCaptureVideoKey2", typeof(byte))
                };

                //Capture image hotkey
                if (usedImageKeys.Count > 0 && !usedImageKeys.Where(x => x != KeysVirtual.None).Except(keysPressed).Any())
                {
                    Debug.WriteLine("Button Global - Capture image");
                    CaptureScreen.CaptureImageProcess();
                }

                //Capture video hotkey
                if (usedVideoKeys.Count > 0 && !usedVideoKeys.Where(x => x != KeysVirtual.None).Except(keysPressed).Any())
                {
                    Debug.WriteLine("Button Global - Capture video");
                    await CaptureScreen.CaptureVideoProcess();
                }
            }
            catch { }
        }
    }
}