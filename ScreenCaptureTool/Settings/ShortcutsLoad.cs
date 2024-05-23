using System;
using System.Diagnostics;
using System.Linq;
using static ArnoldVinkCode.AVClasses;
using static ScreenCapture.AppVariables;

namespace ScreenCapture
{
    partial class WindowMain
    {
        //Load - Application Shortcuts
        public void Shortcuts_Load()
        {
            try
            {
                ShortcutTriggerKeyboard shortcutTrigger = vShortcutTriggers.Where(x => x.Name == "CaptureImage").FirstOrDefault();
                if (shortcutTrigger != null)
                {
                    hotkey_CaptureImage.Set(shortcutTrigger.Trigger);
                }
                shortcutTrigger = vShortcutTriggers.Where(x => x.Name == "CaptureVideo").FirstOrDefault();
                if (shortcutTrigger != null)
                {
                    hotkey_CaptureVideo.Set(shortcutTrigger.Trigger);
                }
            }
            catch (Exception ex)
            {
                Debug.WriteLine("Failed to load application shortcuts: " + ex.Message);
            }
        }
    }
}