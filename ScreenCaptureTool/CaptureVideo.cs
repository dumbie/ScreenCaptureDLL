using ArnoldVinkCode;
using System;
using System.Diagnostics;
using System.IO;
using System.Threading.Tasks;
using static ArnoldVinkCode.AVSettings;
using static ScreenCapture.AppVariables;

namespace ScreenCapture
{
    public partial class CaptureScreen
    {
        //Prepare video capture
        public static async Task CaptureVideoToFile()
        {
            try
            {
                //Stop video capture
                if (CaptureImport.CaptureVideoIsRecording())
                {
                    Debug.WriteLine("Stopping video capturing...");

                    //Request to stop video capture
                    bool captureResultStop = CaptureImport.CaptureVideoStop();
                    Debug.WriteLine("Stopped video capturing: " + captureResultStop);

                    //Reset screen capture resources
                    CaptureImport.CaptureReset();

                    //Play capture sound
                    PlayCaptureSound("VideoStop");

                    return;
                }

                //Screen capture settings
                CaptureSettings captureSettings = new CaptureSettings();
                captureSettings.HDRtoSDR = true;

                //Initialize screen capture
                if (!CaptureImport.CaptureInitialize(captureSettings, out CaptureDetails vCaptureDetails))
                {
                    Debug.WriteLine("Failed to initialize screen capture.");

                    //Play capture sound
                    PlayCaptureSound("CaptureFail");
                    return;
                }
                else
                {
                    //Wait for capturer to have initialized
                    await Task.Delay(500);
                }

                //Set screenshot name
                string fileSaveName = "(" + DateTime.Now.ToShortDateString() + ") " + DateTime.Now.ToString("HH.mm.ss.ffff");
                if (vCaptureDetails.HDREnabled)
                {
                    if (captureSettings.HDRtoSDR)
                    {
                        fileSaveName += " (HDRtoSDR)";
                    }
                    else
                    {
                        fileSaveName += " (HDR)";
                    }
                }
                else
                {
                    fileSaveName += " (SDR)";
                }
                fileSaveName = "\\Capture " + AVFiles.FileNameReplaceInvalidChars(fileSaveName, "-");

                //Check screenshot location
                string fileSaveFolder = SettingLoad(vConfiguration, "ScreenshotLocation", typeof(string));
                if (string.IsNullOrWhiteSpace(fileSaveFolder) || !Directory.Exists(fileSaveFolder))
                {
                    //Check screenshots folder in app directory
                    if (!Directory.Exists("Screenshots"))
                    {
                        Directory.CreateDirectory("Screenshots");
                    }

                    //Set save folder to screenshots in app directory
                    fileSaveFolder = "Screenshots";
                }

                //Combine save path
                string fileSavePath = fileSaveFolder + fileSaveName + ".mp4";

                //Start video capture
                bool captureStarted = CaptureImport.CaptureVideoStart(fileSavePath);

                //Play capture sound
                if (captureStarted)
                {
                    PlayCaptureSound("VideoStart");
                    Debug.WriteLine("Started screen capturing video.");
                }
                else
                {
                    PlayCaptureSound("CaptureFail");
                    Debug.WriteLine("Failed screen capturing video.");
                }
            }
            catch (Exception ex)
            {
                Debug.WriteLine("Screen video capture failed: " + ex.Message);

                //Play capture sound
                PlayCaptureSound("CaptureFail");
            }
        }
    }
}