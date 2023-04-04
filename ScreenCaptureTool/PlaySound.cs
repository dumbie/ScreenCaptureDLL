using ArnoldVinkCode;
using System;
using static ArnoldVinkCode.AVSettings;
using static ScreenCapture.AppVariables;

namespace ScreenCapture
{
    public partial class CaptureScreen
    {
        //Play capture sound
        public static void PlayCaptureSound(bool failSound)
        {
            try
            {
                if (SettingLoad(vConfiguration, "SoundScreenshot", typeof(bool)))
                {
                    AVActions.DispatcherInvoke(delegate
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
                    });
                }
            }
            catch { }
        }
    }
}