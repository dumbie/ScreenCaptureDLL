using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using static ArnoldVinkCode.AVClasses;
using static ArnoldVinkCode.AVInputOutputClass;
using static ArnoldVinkCode.AVInputOutputHotkey;
using static ScreenCapture.AppVariables;

namespace ScreenCapture
{
    public partial class AppHotkeys
    {
        public static async void EventHotkeyPressed(List<KeysVirtual> keysPressed)
        {
            try
            {
                ShortcutTriggerKeyboard shortcutTrigger = vShortcutTriggers.Where(x => x.Name == "CaptureImage").FirstOrDefault();
                if (shortcutTrigger != null)
                {
                    if (CheckHotkeyPress(keysPressed, shortcutTrigger.Trigger))
                    {
                        Debug.WriteLine("Button Global - Capture image");
                        await CaptureScreen.CaptureImageProcess(0);
                        return;
                    }
                }

                shortcutTrigger = vShortcutTriggers.Where(x => x.Name == "CaptureVideo").FirstOrDefault();
                if (shortcutTrigger != null)
                {
                    if (CheckHotkeyPress(keysPressed, shortcutTrigger.Trigger))
                    {
                        Debug.WriteLine("Button Global - Capture video");
                        await CaptureScreen.CaptureVideoProcess(0);
                        return;
                    }
                }
            }
            catch { }
        }
    }
}