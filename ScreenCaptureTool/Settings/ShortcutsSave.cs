using ArnoldVinkCode;
using System;
using System.Diagnostics;
using System.Linq;
using static ArnoldVinkCode.AVClasses;
using static ScreenCapture.AppVariables;

namespace ScreenCapture
{
    partial class WindowMain
    {
        //Save - Application Shortcuts
        public void Shortcuts_Save()
        {
            try
            {
                hotkey_CaptureImage.TriggerChanged += (triggers) =>
                {
                    ShortcutTriggerKeyboard shortcutTrigger = vShortcutTriggers.Where(x => x.Name == "CaptureImage").FirstOrDefault();
                    if (shortcutTrigger != null)
                    {
                        shortcutTrigger.Trigger = triggers;
                        AVJsonFunctions.JsonSaveObject(vShortcutTriggers, @"Profiles\ShortcutKeyboard.json");
                    }
                };
                hotkey_CaptureVideo.TriggerChanged += (triggers) =>
                {
                    ShortcutTriggerKeyboard shortcutTrigger = vShortcutTriggers.Where(x => x.Name == "CaptureVideo").FirstOrDefault();
                    if (shortcutTrigger != null)
                    {
                        shortcutTrigger.Trigger = triggers;
                        AVJsonFunctions.JsonSaveObject(vShortcutTriggers, @"Profiles\ShortcutKeyboard.json");
                    }
                };
            }
            catch (Exception ex)
            {
                Debug.WriteLine("Failed to save the application shortcuts: " + ex.Message);
            }
        }
    }
}