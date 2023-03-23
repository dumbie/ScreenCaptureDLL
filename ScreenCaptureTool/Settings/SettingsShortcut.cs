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
                combobox_SetShortcut_Key0.ItemsSource = Enum.GetValues(typeof(KeysVirtual));
                combobox_SetShortcut_Key0.SelectedItem = KeysVirtual.None;

                combobox_SetShortcut_Key1.ItemsSource = Enum.GetValues(typeof(KeysVirtual));
                combobox_SetShortcut_Key1.SelectedItem = KeysVirtual.None;

                combobox_SetShortcut_Key2.ItemsSource = Enum.GetValues(typeof(KeysVirtual));
                combobox_SetShortcut_Key2.SelectedItem = KeysVirtual.None;
            }
            catch { }
        }

        void Check_SetShortcut_Keys()
        {
            try
            {
                if (!SettingCheck(vConfiguration, "ShortcutSceenshotKey0")) { SettingSave(vConfiguration, "ShortcutSceenshotKey0", (byte)KeysVirtual.None); }
                if (!SettingCheck(vConfiguration, "ShortcutSceenshotKey1")) { SettingSave(vConfiguration, "ShortcutSceenshotKey1", (byte)KeysVirtual.None); }
                if (!SettingCheck(vConfiguration, "ShortcutSceenshotKey2")) { SettingSave(vConfiguration, "ShortcutSceenshotKey2", (byte)KeysVirtual.F12); }
            }
            catch { }
        }

        void Load_SetShortcut_Keys()
        {
            try
            {
                KeysVirtual usedKey0 = (KeysVirtual)SettingLoad(vConfiguration, "ShortcutSceenshotKey0", typeof(byte));
                combobox_SetShortcut_Key0.SelectedItem = usedKey0;

                KeysVirtual usedKey1 = (KeysVirtual)SettingLoad(vConfiguration, "ShortcutSceenshotKey1", typeof(byte));
                combobox_SetShortcut_Key1.SelectedItem = usedKey1;

                KeysVirtual usedKey2 = (KeysVirtual)SettingLoad(vConfiguration, "ShortcutSceenshotKey2", typeof(byte));
                combobox_SetShortcut_Key2.SelectedItem = usedKey2;
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
                KeysVirtual usedKey0 = (KeysVirtual)combobox_SetShortcut_Key0.SelectedItem;
                KeysVirtual usedKey1 = (KeysVirtual)combobox_SetShortcut_Key1.SelectedItem;
                KeysVirtual usedKey2 = (KeysVirtual)combobox_SetShortcut_Key2.SelectedItem;

                //Save shortcut keys
                SettingSave(vConfiguration, "ShortcutSceenshotKey0", (byte)usedKey0);
                SettingSave(vConfiguration, "ShortcutSceenshotKey1", (byte)usedKey1);
                SettingSave(vConfiguration, "ShortcutSceenshotKey2", (byte)usedKey2);
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
                combobox_SetShortcut_Key0.SelectedItem = KeysVirtual.None;
                combobox_SetShortcut_Key1.SelectedItem = KeysVirtual.None;
                combobox_SetShortcut_Key2.SelectedItem = KeysVirtual.None;
                vComboboxSaveEnabled = true;

                //Save shortcut keys
                SettingSave(vConfiguration, "ShortcutSceenshotKey0", (byte)KeysVirtual.None);
                SettingSave(vConfiguration, "ShortcutSceenshotKey1", (byte)KeysVirtual.None);
                SettingSave(vConfiguration, "ShortcutSceenshotKey2", (byte)KeysVirtual.None);
                Debug.WriteLine("Unmapped shortcut: screenshot");
            }
            catch { }
        }
    }
}