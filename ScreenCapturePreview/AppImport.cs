using System;
using System.Runtime.InteropServices;

namespace ScreenCapture
{
    class AppImport
    {
        //Register Hotkey
        [DllImport("user32.dll")]
        public static extern bool RegisterHotKey(IntPtr hWnd, int id, int fsModifiers, byte bVk);

        //Unregister Hotkey
        [DllImport("user32.dll")]
        public static extern bool UnregisterHotKey(IntPtr hWnd, int id);

        public static int HOTKEY_ID = 0x9000;
        public static int WM_HOTKEY = 0x312;
    }
}