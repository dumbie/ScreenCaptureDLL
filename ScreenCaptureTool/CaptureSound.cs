using System;
using static ScreenCapture.AppVariables;

namespace ScreenCapture
{
    public partial class CaptureScreen
    {
        //Play capture sound
        public static void CaptureSound(bool failSound)
        {
            try
            {
                if (!failSound)
                {
                    vWindowsMediaPlayer.Volume = 1.0;
                    vWindowsMediaPlayer.Open(new Uri("Resources\\Screenshot.mp3", UriKind.RelativeOrAbsolute));
                    vWindowsMediaPlayer.Play();
                }
                else
                {
                    vWindowsMediaPlayer.Volume = 1.0;
                    vWindowsMediaPlayer.Open(new Uri("Resources\\ScreenshotFail.mp3", UriKind.RelativeOrAbsolute));
                    vWindowsMediaPlayer.Play();
                }
            }
            catch { }
        }
    }
}