using ArnoldVinkCode;
using System;
using System.Diagnostics;
using static ArnoldVinkCode.AVClasses;
using static ArnoldVinkCode.AVJsonFunctions;
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
                keyboard_CaptureImage.TriggerChanged += Shortcut_Keyboard_TriggerChanged;
                keyboard_CaptureVideo.TriggerChanged += Shortcut_Keyboard_TriggerChanged;
            }
            catch (Exception ex)
            {
                Debug.WriteLine("Failed to save the application shortcuts: " + ex.Message);
            }
        }

        void Shortcut_Keyboard_TriggerChanged(ShortcutTriggerKeyboard triggers)
        {
            try
            {
                if (vShortcutTriggers.ListReplaceFirstItem(x => x.Name == triggers.Name, triggers))
                {
                    JsonSaveObject(vShortcutTriggers, @"Profiles\User\CaptureShortcutKeyboard.json");
                }
            }
            catch { }
        }
    }
}