using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Reflection;
using static ArnoldVinkCode.AVInteropDll;
using static ArnoldVinkCode.AVProcess;

namespace ScreenCapture
{
    public partial class AppCheck
    {
        public static void StartupCheck(string appName, ProcessPriority priorityLevel)
        {
            try
            {
                Debug.WriteLine("Checking application status.");

                //Get current process information
                ProcessMulti currentProcess = Get_ProcessMultiCurrent();
                List<ProcessMulti> activeProcesses = Get_ProcessesMultiByName(currentProcess.ExeNameNoExt, true);

                //Check if application is already running
                if (activeProcesses.Count > 1)
                {
                    Debug.WriteLine("Application " + appName + " is already running, closing the process");
                    Environment.Exit(0);
                    return;
                }

                //Set the working directory to executable directory
                try
                {
                    Directory.SetCurrentDirectory(Path.GetDirectoryName(Assembly.GetEntryAssembly().Location));
                }
                catch { }

                //Set the application priority level
                try
                {
                    currentProcess.Priority = priorityLevel;
                }
                catch { }
            }
            catch { }
        }
    }
}