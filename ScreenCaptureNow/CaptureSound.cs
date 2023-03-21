using System;
using System.Threading.Tasks;

namespace ScreenCapture
{
    public partial class CaptureScreen
    {
        //Play capture sound
        public static async Task CaptureSound(bool failSound)
        {
            try
            {
                if (!failSound)
                {
                    vWindowsMediaPlayer.Volume = 1.0;
                    vWindowsMediaPlayer.Open(new Uri("Assets\\Screenshot.mp3", UriKind.RelativeOrAbsolute));
                    vWindowsMediaPlayer.Play();
                }
                else
                {
                    vWindowsMediaPlayer.Volume = 1.0;
                    vWindowsMediaPlayer.Open(new Uri("Assets\\ScreenshotFail.mp3", UriKind.RelativeOrAbsolute));
                    vWindowsMediaPlayer.Play();
                }

                //Allow sound to finish
                await Task.Delay(1000);
            }
            catch { }
        }
    }
}