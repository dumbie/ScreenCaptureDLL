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

                combobox_SetShortcutCaptureVideo_Key0.ItemsSource = Enum.GetValues(typeof(KeysVirtual));
                combobox_SetShortcutCaptureVideo_Key0.SelectedItem = KeysVirtual.None;
                combobox_SetShortcutCaptureVideo_Key1.ItemsSource = Enum.GetValues(typeof(KeysVirtual));
                combobox_SetShortcutCaptureVideo_Key1.SelectedItem = KeysVirtual.None;
                combobox_SetShortcutCaptureVideo_Key2.ItemsSource = Enum.GetValues(typeof(KeysVirtual));
                combobox_SetShortcutCaptureVideo_Key2.SelectedItem = KeysVirtual.None;
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
                KeysVirtual usedImageKey0 = (KeysVirtual)SettingLoad(vConfiguration, "ShortcutCaptureImageKey0", typeof(byte));
                combobox_SetShortcutCaptureImage_Key0.SelectedItem = usedImageKey0;
                KeysVirtual usedImageKey1 = (KeysVirtual)SettingLoad(vConfiguration, "ShortcutCaptureImageKey1", typeof(byte));
                combobox_SetShortcutCaptureImage_Key1.SelectedItem = usedImageKey1;
                KeysVirtual usedImageKey2 = (KeysVirtual)SettingLoad(vConfiguration, "ShortcutCaptureImageKey2", typeof(byte));
                combobox_SetShortcutCaptureImage_Key2.SelectedItem = usedImageKey2;

                KeysVirtual usedVideoKey0 = (KeysVirtual)SettingLoad(vConfiguration, "ShortcutCaptureVideoKey0", typeof(byte));
                combobox_SetShortcutCaptureVideo_Key0.SelectedItem = usedVideoKey0;
                KeysVirtual usedVideoKey1 = (KeysVirtual)SettingLoad(vConfiguration, "ShortcutCaptureVideoKey1", typeof(byte));
                combobox_SetShortcutCaptureVideo_Key1.SelectedItem = usedVideoKey1;
                KeysVirtual usedVideoKey2 = (KeysVirtual)SettingLoad(vConfiguration, "ShortcutCaptureVideoKey2", typeof(byte));
                combobox_SetShortcutCaptureVideo_Key2.SelectedItem = usedVideoKey2;
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
                KeysVirtual usedImageKey0 = (KeysVirtual)combobox_SetShortcutCaptureImage_Key0.SelectedItem;
                KeysVirtual usedImageKey1 = (KeysVirtual)combobox_SetShortcutCaptureImage_Key1.SelectedItem;
                KeysVirtual usedImageKey2 = (KeysVirtual)combobox_SetShortcutCaptureImage_Key2.SelectedItem;
                KeysVirtual usedVideoKey0 = (KeysVirtual)combobox_SetShortcutCaptureVideo_Key0.SelectedItem;
                KeysVirtual usedVideoKey1 = (KeysVirtual)combobox_SetShortcutCaptureVideo_Key1.SelectedItem;
                KeysVirtual usedVideoKey2 = (KeysVirtual)combobox_SetShortcutCaptureVideo_Key2.SelectedItem;

                //Save shortcut keys
                SettingSave(vConfiguration, "ShortcutCaptureImageKey0", (byte)usedImageKey0);
                SettingSave(vConfiguration, "ShortcutCaptureImageKey1", (byte)usedImageKey1);
                SettingSave(vConfiguration, "ShortcutCaptureImageKey2", (byte)usedImageKey2);
                SettingSave(vConfiguration, "ShortcutCaptureVideoKey0", (byte)usedVideoKey0);
                SettingSave(vConfiguration, "ShortcutCaptureVideoKey1", (byte)usedVideoKey1);
                SettingSave(vConfiguration, "ShortcutCaptureVideoKey2", (byte)usedVideoKey2);

                Debug.WriteLine("Saving shortcut screen image to " + usedImageKey0 + "/" + usedImageKey1 + "/" + usedImageKey2);
                Debug.WriteLine("Saving shortcut screen video to " + usedVideoKey0 + "/" + usedVideoKey1 + "/" + usedVideoKey2);
            }
            catch { }
        }

        private void Button_SetShortcut_Unmap_Image_Click(object sender, RoutedEventArgs e)
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
                Debug.WriteLine("Unmapped shortcut: Capture image");
            }
            catch { }
        }

        private void Button_SetShortcut_Unmap_Video_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                //Reset combobox selection
                vComboboxSaveEnabled = false;
                combobox_SetShortcutCaptureVideo_Key0.SelectedItem = KeysVirtual.None;
                combobox_SetShortcutCaptureVideo_Key1.SelectedItem = KeysVirtual.None;
                combobox_SetShortcutCaptureVideo_Key2.SelectedItem = KeysVirtual.None;
                vComboboxSaveEnabled = true;

                //Save shortcut keys
                SettingSave(vConfiguration, "ShortcutCaptureVideoKey0", (byte)KeysVirtual.None);
                SettingSave(vConfiguration, "ShortcutCaptureVideoKey1", (byte)KeysVirtual.None);
                SettingSave(vConfiguration, "ShortcutCaptureVideoKey2", (byte)KeysVirtual.None);
                Debug.WriteLine("Unmapped shortcut: Capture video");
            }
            catch { }
        }
    }
}