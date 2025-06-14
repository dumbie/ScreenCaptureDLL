using ArnoldVinkCode;
using ScreenCaptureImport;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Threading.Tasks;
using static ArnoldVinkCode.AVProcess;
using static ArnoldVinkCode.AVSettings;
using static ArnoldVinkCode.AVUpdate;
using static ScreenCapture.AppClasses;
using static ScreenCapture.AppVariables;

namespace ScreenCapture
{
    public class AppStartup
    {
        public static async Task Startup(string[] launchArgs)
        {
            try
            {
                Debug.WriteLine("Welcome to application.");

                //Application initialize settings
                vWindowMain.Settings_Check();

                //Check if capture process is running
                List<ProcessMulti> screenCaptureProcess = Get_ProcessesMultiByName("ScreenCaptureTool", true);
                bool processRunningTool = screenCaptureProcess.Count(x => string.IsNullOrWhiteSpace(x.Argument)) > 1;
                bool processRunningVideo = screenCaptureProcess.Count(x => x.Argument.ToLower() == "-video") > 1;

                //Check launch arguments
                CaptureTypes launchAction = CaptureTypes.None;
                if (launchArgs != null && launchArgs.Any())
                {
                    foreach (string launchArgument in launchArgs)
                    {
                        try
                        {
                            //Convert launch argument to lower
                            string launchArgumentLower = launchArgument.ToLower();
                            if (launchArgumentLower.StartsWith("-image"))
                            {
                                launchAction = CaptureTypes.Image;
                            }
                            else if (launchArgumentLower.StartsWith("-video"))
                            {
                                launchAction = CaptureTypes.Video;
                            }
                        }
                        catch { }
                    }
                }

                //Debug launch actions
                //launchAction = CaptureTypes.Video;
                //launchAction = CaptureTypes.Image;

                //Check launch action
                if (launchAction == CaptureTypes.Image)
                {
                    Debug.WriteLine("Screen Capture Tool image.");

                    //Capture image to file
                    CaptureResult captureResult = CaptureScreen.CaptureImageToFile();
                    Debug.WriteLine("Capture image result: " + captureResult.Status + " / " + captureResult.ResultCode + " / " + captureResult.Message);

                    //Check capture result
                    if (captureResult.Status == CaptureStatus.Success)
                    {
                        //Show capture overlay window
                        if (SettingLoad(vConfigurationScreenCaptureTool, "OverlayShowScreenshot", typeof(bool)))
                        {
                            vWindowOverlay.ShowOverlay(CaptureTypes.Image, captureResult.Message);
                            await Task.Delay(2500);
                            vWindowOverlay.Hide();
                        }
                    }
                    else
                    {
                        //Show capture overlay window
                        vWindowOverlay.ShowOverlay(CaptureTypes.Failed, captureResult.Message);
                        await Task.Delay(2500);
                        vWindowOverlay.Hide();
                    }

                    //Close application
                    await AppExit.Exit();
                }
                else if (launchAction == CaptureTypes.Video)
                {
                    //Check if application is already running
                    if (processRunningVideo)
                    {
                        Debug.WriteLine("Application is already running, closing the process");
                        await AppExit.Exit();
                        return;
                    }

                    Debug.WriteLine("Screen Capture Tool video.");

                    //Capture video to file
                    CaptureResult captureResult = CaptureScreen.StartCaptureVideoToFile();
                    Debug.WriteLine("Capture video result: " + captureResult.Status + " / " + captureResult.ResultCode + " / " + captureResult.Message);

                    //Check capture result
                    if (captureResult.Status == CaptureStatus.Success)
                    {
                        //Show capture overlay window
                        if (SettingLoad(vConfigurationScreenCaptureTool, "OverlayShowRecording", typeof(bool)))
                        {
                            vWindowOverlay.ShowOverlay(CaptureTypes.Video, captureResult.Message);
                        }

                        //Create application tray menu
                        AppTrayMenuCapture.Application_CreateTrayMenu();

                        //Start keyboard hotkeys
                        AVInputOutputHotkeyHook.Start();
                        AVInputOutputHotkeyHook.EventHotkeyPressedList += AppHotkeys.EventHotkeyPressed;

                        //Enable socket server
                        await SocketServer.EnableSocketServer();
                    }
                    else
                    {
                        //Show capture overlay window
                        vWindowOverlay.ShowOverlay(CaptureTypes.Failed, captureResult.Message);
                        await Task.Delay(2500);
                        vWindowOverlay.Hide();

                        //Close application
                        await AppExit.Exit();
                    }
                }
                else
                {
                    //Check if application is already running
                    if (processRunningTool)
                    {
                        Debug.WriteLine("Application is already running, closing the process");
                        await AppExit.Exit();
                        return;
                    }

                    Debug.WriteLine("Screen Capture Tool launch.");

                    //Clean application update files
                    await UpdateCleanup();

                    //Check for available application update
                    await UpdateCheck("dumbie", "ScreenCaptureTool", true);

                    //Application initialize settings
                    vWindowMain.Settings_Load();
                    vWindowMain.Settings_Save();

                    //Application initialize shortcuts
                    vWindowMain.Shortcuts_Check();
                    vWindowMain.Shortcuts_Load();
                    vWindowMain.Shortcuts_Save();

                    //Create application tray menu
                    AppTrayMenuTool.Application_CreateTrayMenu();

                    //Check settings if window needs to be shown
                    if (SettingLoad(vConfigurationScreenCaptureTool, "AppFirstLaunch", typeof(bool)))
                    {
                        Debug.WriteLine("First launch showing the window.");
                        vWindowMain.Application_ShowWindow();
                    }

                    //Start keyboard hotkeys
                    AVInputOutputHotkeyHook.Start();
                    AVInputOutputHotkeyHook.EventHotkeyPressedList += AppHotkeys.EventHotkeyPressed;

                    //Enable socket server
                    await SocketServer.EnableSocketServer();
                }
            }
            catch { }
        }
    }
}