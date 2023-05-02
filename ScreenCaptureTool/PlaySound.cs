using ArnoldVinkCode;
using System;
using static ArnoldVinkCode.AVSettings;
using static ScreenCapture.AppVariables;

namespace ScreenCapture
{
    public partial class CaptureScreen
    {
        //Play capture sound
        public static void PlayCaptureSound(string soundName)
        {
            try
            {
                if (SettingLoad(vConfiguration, "SoundScreenshot", typeof(bool)))
                {
                    AVActions.DispatcherInvoke(delegate
                    {
                        vWindowsMediaPlayer.Volume = 1.00;
                        vWindowsMediaPlayer.Open(new Uri("Assets\\" + soundName + ".mp3", UriKind.RelativeOrAbsolute));
                        vWindowsMediaPlayer.Play();
                    });
                }
            }
            catch { }
        }
    }
}