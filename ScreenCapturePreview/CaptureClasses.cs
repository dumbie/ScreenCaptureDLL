namespace ScreenCapturePreview
{
    public class CaptureClasses
    {
        public struct CaptureSettings
        {
            public int MonitorId { get; set; }
            public int MaxPixelDimension { get; set; }
            public bool HDRtoSDR { get; set; }
            public float HDRBrightness { get; set; }
            public float Vibrance { get; set; }
            public float Saturate { get; set; }
            public float Temperature { get; set; }
            public float RedChannel { get; set; }
            public float GreenChannel { get; set; }
            public float BlueChannel { get; set; }
            public float Brightness { get; set; }
            public float Contrast { get; set; }
            public float Gamma { get; set; }
        }

        public struct CaptureDetails
        {
            public int Width { get; set; }
            public int Height { get; set; }
            public bool HDREnabled { get; set; }
            public float SDRWhiteLevel { get; set; }
            public int PixelByteSize { get; set; }
            public int WidthByteSize { get; set; }
            public int TotalByteSize { get; set; }
        }
    }
}