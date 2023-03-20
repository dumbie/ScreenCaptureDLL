using System.Collections.Generic;
using System.Diagnostics;
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
                bool altPressed = keysPressed.Contains(KeysVirtual.AltLeft);
                bool controlPressed = keysPressed.Contains(KeysVirtual.ControlLeft);
                bool shiftPressed = keysPressed.Contains(KeysVirtual.ShiftLeft);
                bool windowsPressed = keysPressed.Contains(KeysVirtual.WindowsLeft);
                bool modifierKeyPressed = altPressed || controlPressed || shiftPressed || windowsPressed;

                if (!modifierKeyPressed && keysPressed.Contains(KeysVirtual.F12))
                {
                    //Capture screenshot hotkey
                    if (SettingLoad(vConfiguration, "ShortcutScreenshotKeyboard", typeof(bool)))
                    {
                        Debug.WriteLine("Button Global - Capture screenshot");
                        await CaptureScreen.CaptureScreenToFile();
                    }
                }
            }
            catch { }
        }
    }
}