using System;
using System.Diagnostics;
using System.Linq;
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
                keyboard_CaptureImage.Set(vShortcutTriggers.FirstOrDefault(x => x.Name == keyboard_CaptureImage.TriggerName));
                keyboard_CaptureVideo.Set(vShortcutTriggers.FirstOrDefault(x => x.Name == keyboard_CaptureVideo.TriggerName));
            }
            catch (Exception ex)
            {
                Debug.WriteLine("Failed to load application shortcuts: " + ex.Message);
            }
        }
    }
}