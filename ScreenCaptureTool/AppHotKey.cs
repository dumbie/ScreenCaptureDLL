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
                //Load set shortcut keys
                KeysVirtual usedKey0 = (KeysVirtual)SettingLoad(vConfiguration, "ShortcutCaptureImageKey0", typeof(byte));
                KeysVirtual usedKey1 = (KeysVirtual)SettingLoad(vConfiguration, "ShortcutCaptureImageKey1", typeof(byte));
                KeysVirtual usedKey2 = (KeysVirtual)SettingLoad(vConfiguration, "ShortcutCaptureImageKey2", typeof(byte));
                List<KeysVirtual> shortcutKeys = new List<KeysVirtual>();
                if (usedKey0 != KeysVirtual.None)
                {
                    shortcutKeys.Add(usedKey0);
                }
                if (usedKey1 != KeysVirtual.None)
                {
                    shortcutKeys.Add(usedKey1);
                }
                if (usedKey2 != KeysVirtual.None)
                {
                    shortcutKeys.Add(usedKey2);
                }

                //Check if shortcut is set
                if (shortcutKeys.Count == 0) { return; }

                //Capture screenshot hotkey
                if (!shortcutKeys.Except(keysPressed).Any())
                {
                    Debug.WriteLine("Button Global - Capture screenshot");
                    await CaptureScreen.CaptureScreenToFile();
                }
            }
            catch { }
        }
    }
}