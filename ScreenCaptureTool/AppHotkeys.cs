using System.Diagnostics;
using System.Threading.Tasks;
using static ArnoldVinkCode.AVActions;
using static ArnoldVinkCode.AVClasses;
using static ArnoldVinkCode.AVInputOutputHotkeyHook;
using static ScreenCapture.AppVariables;

namespace ScreenCapture
{
    public partial class AppHotkeys
    {
        public static void EventHotkeyPressed(bool[] keysPressed)
        {
            try
            {
                foreach (ShortcutTriggerKeyboard shortcutTrigger in vShortcutTriggers)
                {
                    if (shortcutTrigger.Name == "CaptureImage")
                    {
                        if (CheckHotkeyPressed(keysPressed, shortcutTrigger.Trigger))
                        {
                            Debug.WriteLine("Button Global - Capture image");
                            async Task TaskAction()
                            {
                                await CaptureScreen.CaptureImageProcess(0);
                            }
                            TaskStartBackground(TaskAction);
                            return;
                        }
                    }
                    else if (shortcutTrigger.Name == "CaptureVideo")
                    {
                        if (CheckHotkeyPressed(keysPressed, shortcutTrigger.Trigger))
                        {
                            Debug.WriteLine("Button Global - Capture video");
                            async Task TaskAction()
                            {
                                await CaptureScreen.CaptureVideoProcess(0);
                            }
                            TaskStartBackground(TaskAction);
                            return;
                        }
                    }
                }
            }
            catch { }
        }
    }
}