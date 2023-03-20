using System;
using System.Diagnostics;

namespace ScreenCapture
{
    partial class WindowMain
    {
        //Save - Application Settings
        void Settings_Save()
        {
            try
            {
            }
            catch (Exception ex)
            {
                Debug.WriteLine("Failed to save the application settings: " + ex.Message);
            }
        }
    }
}