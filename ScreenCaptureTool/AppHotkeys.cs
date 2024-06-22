using System.Collections.Generic;
using System.Diagnostics;
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
                foreach (ShortcutTriggerKeyboard shortcutTrigger in vShortcutTriggers)
                {
                    if (shortcutTrigger.Name == "CaptureImage")
                    {
                        if (CheckHotkeyPress(keysPressed, shortcutTrigger.Trigger))
                        {
                            Debug.WriteLine("Button Global - Capture image");
                            await CaptureScreen.CaptureImageProcess(0);
                            return;
                        }
                    }
                    else if (shortcutTrigger.Name == "CaptureVideo")
                    {
                        if (CheckHotkeyPress(keysPressed, shortcutTrigger.Trigger))
                        {
                            Debug.WriteLine("Button Global - Capture video");
                            await CaptureScreen.CaptureVideoProcess(0);
                            return;
                        }
                    }
                }
            }
            catch { }
        }
    }
}