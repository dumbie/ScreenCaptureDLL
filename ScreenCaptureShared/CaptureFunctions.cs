using System.IO;

namespace ScreenCapture
{
    public class CaptureFunctions
    {
        //Replace invalid file characters
        public static string FileNameReplaceInvalidChars(string fileName)
        {
            return string.Join("-", fileName.Split(Path.GetInvalidFileNameChars()));
        }
    }
}