using System;
using System.Diagnostics;
using System.Threading.Tasks;

namespace ScreenCapture
{
    partial class WindowMain
    {
        //Load - Application Settings
        async Task Settings_Load()
        {
            try
            {
            }
            catch (Exception ex)
            {
                Debug.WriteLine("Failed to load application settings: " + ex.Message);
            }
        }
    }
}