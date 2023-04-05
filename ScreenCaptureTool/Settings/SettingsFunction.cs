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
        private void button_ScreenshotLocationChange_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                using (FolderBrowserDialog folderBrowserDialog = new FolderBrowserDialog())
                {
                    folderBrowserDialog.ShowDialog();
                    if (!string.IsNullOrWhiteSpace(folderBrowserDialog.SelectedPath))
                    {
                        Debug.WriteLine("Screenshot location selected: " + folderBrowserDialog.SelectedPath);
                        SettingSave(vConfiguration, "ScreenshotLocation", folderBrowserDialog.SelectedPath);
                        textblock_ScreenshotLocation.Text = textblock_ScreenshotLocation.Tag + folderBrowserDialog.SelectedPath;
                    }
                }
            }
            catch { }
        }

        //Open capture location
        private void button_ScreenshotLocationOpen_Click(object sender, RoutedEventArgs e)
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
                string screenshotSaveFolder = SettingLoad(vConfiguration, "ScreenshotLocation", typeof(string));
                if (!Directory.Exists(screenshotSaveFolder))
                {
                    //Check screenshots folder in app directory
                    if (!Directory.Exists("Screenshots"))
                    {
                        Directory.CreateDirectory("Screenshots");
                    }
                    Process.Start("Screenshots");
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