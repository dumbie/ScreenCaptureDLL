﻿namespace ScreenCapture
{
    //Enumerators
    public enum ImageFormats : int
    {
        JXR = 0,
        JPG = 1,
        PNG = 2,
        BMP = 3,
        TIF = 4,
        HEIF = 5
    }

    //Structures
    public struct CaptureSettings
    {
        public CaptureSettings() { }
        public int MonitorId { get; set; } = 0;
        public int MaxPixelDimension { get; set; } = 0;
        public bool MouseIgnoreMovement { get; set; } = false;
        public bool MouseDrawCursor { get; set; } = true;
        public bool HDRtoSDR { get; set; } = false;
        public float HDRPaperWhite { get; set; } = 80.0F;
        public float HDRMaximumNits { get; set; } = 1000.0F;
        public float Saturation { get; set; } = 1.0F;
        public float RedChannel { get; set; } = 1.0F;
        public float GreenChannel { get; set; } = 1.0F;
        public float BlueChannel { get; set; } = 1.0F;
        public float Brightness { get; set; } = 1.0F;
        public float Contrast { get; set; } = 0.0F;
        public float Gamma { get; set; } = 1.0F;
        public float Blur { get; set; } = 0.0F;
    }

    public struct CaptureDetails
    {
        public int OriginalWidth { get; set; }
        public int OriginalHeight { get; set; }
        public int OutputWidth { get; set; }
        public int OutputHeight { get; set; }
        public int RefreshRate { get; set; }
        public bool HDREnabled { get; set; }
        public float SDRWhiteLevel { get; set; }
        public int PixelByteSize { get; set; }
        public int WidthByteSize { get; set; }
        public int TotalByteSize { get; set; }
    }
}