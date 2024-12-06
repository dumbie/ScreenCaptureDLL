using ArnoldVinkCode;
using System;
using System.Diagnostics;
using System.Linq;
using static ArnoldVinkCode.AVClasses;
using static ArnoldVinkCode.AVInputOutputClass;
using static ScreenCapture.AppVariables;

namespace ScreenCapture
{
    public partial class WindowMain
    {
        public void Shortcuts_Check()
        {
            try
            {
                if (!vShortcutTriggers.Any(x => x.Name == "CaptureImage"))
                {
                    ShortcutTriggerKeyboard shortcutTrigger = new ShortcutTriggerKeyboard();
                    shortcutTrigger.Name = "CaptureImage";
                    shortcutTrigger.Trigger = [KeysVirtual.AltLeft, KeysVirtual.None, KeysVirtual.F12];
                    vShortcutTriggers.Add(shortcutTrigger);
                    AVJsonFunctions.JsonSaveObject(vShortcutTriggers, @"Profiles\User\CaptureShortcutKeyboard.json");
                }
                if (!vShortcutTriggers.Any(x => x.Name == "CaptureVideo"))
                {
                    ShortcutTriggerKeyboard shortcutTrigger = new ShortcutTriggerKeyboard();
                    shortcutTrigger.Name = "CaptureVideo";
                    shortcutTrigger.Trigger = [KeysVirtual.AltLeft, KeysVirtual.None, KeysVirtual.F11];
                    vShortcutTriggers.Add(shortcutTrigger);
                    AVJsonFunctions.JsonSaveObject(vShortcutTriggers, @"Profiles\User\CaptureShortcutKeyboard.json");
                }
            }
            catch (Exception ex)
            {
                Debug.WriteLine("Failed to check the application shortcuts: " + ex.Message);
            }
        }
    }
}