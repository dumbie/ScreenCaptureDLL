using System.Diagnostics;
using System.IO;
using System.Windows;
using System.Windows.Forms;
using static ArnoldVinkCode.AVSettings;
using static ScreenCapture.AppVariables;

namespace ScreenCapture
{
    partial class WindowMain
    {
        //Change screenshot location
        private void button_CaptureLocationChange_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                using (FolderBrowserDialog folderBrowserDialog = new FolderBrowserDialog())
                {
                    folderBrowserDialog.ShowDialog();
                    if (!string.IsNullOrWhiteSpace(folderBrowserDialog.SelectedPath))
                    {
                        Debug.WriteLine("Screenshot location selected: " + folderBrowserDialog.SelectedPath);
                        SettingSave(vConfiguration, "CaptureLocation", folderBrowserDialog.SelectedPath);
                        textblock_CaptureLocation.Text = textblock_CaptureLocation.Tag + folderBrowserDialog.SelectedPath;
                    }
                }
            }
            catch { }
        }

        //Open capture location
        private void button_CaptureLocationOpen_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                OpenCaptureLocation();
            }
            catch { }
        }

        //Open capture location
        public static void OpenCaptureLocation()
        {
            try
            {
                //Check screenshot location
                string screenshotSaveFolder = SettingLoad(vConfiguration, "CaptureLocation", typeof(string));
                if (!Directory.Exists(screenshotSaveFolder))
                {
                    //Check captures folder in app directory
                    if (!Directory.Exists("Captures"))
                    {
                        Directory.CreateDirectory("Captures");
                    }
                    Process.Start("Captures");
                }
                else
                {
                    Process.Start(screenshotSaveFolder);
                }
            }
            catch { }
        }
    }
}