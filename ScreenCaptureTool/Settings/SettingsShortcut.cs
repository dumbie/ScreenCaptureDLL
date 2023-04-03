using System;
using System.Diagnostics;
using System.Windows;
using System.Windows.Controls;
using static ArnoldVinkCode.AVInputOutputClass;
using static ArnoldVinkCode.AVSettings;
using static ScreenCapture.AppVariables;

namespace ScreenCapture
{
    partial class WindowMain
    {
        void Load_SetShortcut_Items()
        {
            try
            {
                combobox_SetShortcutCaptureImage_Key0.ItemsSource = Enum.GetValues(typeof(KeysVirtual));
                combobox_SetShortcutCaptureImage_Key0.SelectedItem = KeysVirtual.None;

                combobox_SetShortcutCaptureImage_Key1.ItemsSource = Enum.GetValues(typeof(KeysVirtual));
                combobox_SetShortcutCaptureImage_Key1.SelectedItem = KeysVirtual.None;

                combobox_SetShortcutCaptureImage_Key2.ItemsSource = Enum.GetValues(typeof(KeysVirtual));
                combobox_SetShortcutCaptureImage_Key2.SelectedItem = KeysVirtual.None;
            }
            catch { }
        }

        void Check_SetShortcut_Keys()
        {
            try
            {
                if (!SettingCheck(vConfiguration, "ShortcutCaptureImageKey0")) { SettingSave(vConfiguration, "ShortcutCaptureImageKey0", (byte)KeysVirtual.AltLeft); }
                if (!SettingCheck(vConfiguration, "ShortcutCaptureImageKey1")) { SettingSave(vConfiguration, "ShortcutCaptureImageKey1", (byte)KeysVirtual.None); }
                if (!SettingCheck(vConfiguration, "ShortcutCaptureImageKey2")) { SettingSave(vConfiguration, "ShortcutCaptureImageKey2", (byte)KeysVirtual.F12); }

                if (!SettingCheck(vConfiguration, "ShortcutCaptureVideoKey0")) { SettingSave(vConfiguration, "ShortcutCaptureVideoKey0", (byte)KeysVirtual.ControlLeft); }
                if (!SettingCheck(vConfiguration, "ShortcutCaptureVideoKey1")) { SettingSave(vConfiguration, "ShortcutCaptureVideoKey1", (byte)KeysVirtual.None); }
                if (!SettingCheck(vConfiguration, "ShortcutCaptureVideoKey2")) { SettingSave(vConfiguration, "ShortcutCaptureVideoKey2", (byte)KeysVirtual.F12); }
            }
            catch { }
        }

        void Load_SetShortcut_Keys()
        {
            try
            {
                KeysVirtual usedKey0 = (KeysVirtual)SettingLoad(vConfiguration, "ShortcutCaptureImageKey0", typeof(byte));
                combobox_SetShortcutCaptureImage_Key0.SelectedItem = usedKey0;

                KeysVirtual usedKey1 = (KeysVirtual)SettingLoad(vConfiguration, "ShortcutCaptureImageKey1", typeof(byte));
                combobox_SetShortcutCaptureImage_Key1.SelectedItem = usedKey1;

                KeysVirtual usedKey2 = (KeysVirtual)SettingLoad(vConfiguration, "ShortcutCaptureImageKey2", typeof(byte));
                combobox_SetShortcutCaptureImage_Key2.SelectedItem = usedKey2;
            }
            catch { }
        }

        private void Combobox_SetShortcut_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            try
            {
                //Check if combobox saving is enabled
                if (!vComboboxSaveEnabled) { return; }

                //Get selected keypad buttons
                KeysVirtual usedKey0 = (KeysVirtual)combobox_SetShortcutCaptureImage_Key0.SelectedItem;
                KeysVirtual usedKey1 = (KeysVirtual)combobox_SetShortcutCaptureImage_Key1.SelectedItem;
                KeysVirtual usedKey2 = (KeysVirtual)combobox_SetShortcutCaptureImage_Key2.SelectedItem;

                //Save shortcut keys
                SettingSave(vConfiguration, "ShortcutCaptureImageKey0", (byte)usedKey0);
                SettingSave(vConfiguration, "ShortcutCaptureImageKey1", (byte)usedKey1);
                SettingSave(vConfiguration, "ShortcutCaptureImageKey2", (byte)usedKey2);
                Debug.WriteLine("Saving shortcut screenshot to " + usedKey0 + "/" + usedKey1 + "/" + usedKey2);
            }
            catch { }
        }

        private void Button_SetShortcut_Unmap_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                //Reset combobox selection
                vComboboxSaveEnabled = false;
                combobox_SetShortcutCaptureImage_Key0.SelectedItem = KeysVirtual.None;
                combobox_SetShortcutCaptureImage_Key1.SelectedItem = KeysVirtual.None;
                combobox_SetShortcutCaptureImage_Key2.SelectedItem = KeysVirtual.None;
                vComboboxSaveEnabled = true;

                //Save shortcut keys
                SettingSave(vConfiguration, "ShortcutCaptureImageKey0", (byte)KeysVirtual.None);
                SettingSave(vConfiguration, "ShortcutCaptureImageKey1", (byte)KeysVirtual.None);
                SettingSave(vConfiguration, "ShortcutCaptureImageKey2", (byte)KeysVirtual.None);
                Debug.WriteLine("Unmapped shortcut: screenshot");
            }
            catch { }
        }
    }
}