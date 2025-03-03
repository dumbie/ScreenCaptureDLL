using System.Runtime.InteropServices;

namespace ScreenCaptureImport
{
    //Enumerators
    public enum CaptureStatus : int
    {
        Success = 0,
        Failed = 1,
        Busy = 2
    }

    public enum ImageFormats : int
    {
        JXR = 0,
        JPG = 1,
        PNG = 2,
        BMP = 3,
        TIF = 4,
        HEIF = 5
    }

    public enum VideoRateControls : int
    {
        VBR = 0,
        CBR = 1
    }

    public enum VideoFormats : int
    {
        H264 = 0,
        HEVC = 1,
        AV1 = 2
    }

    public enum AudioFormats : int
    {
        MP3 = 0,
        AAC = 1,
        FLAC = 2
    }

    //Structures
    public struct MediaSettings
    {
        public MediaSettings() { }
        public AudioFormats AudioFormat { get; set; } = AudioFormats.AAC;
        public int AudioChannels { get; set; } = 2;
        public int AudioBitRate { get; set; } = 192;
        public int AudioBitDepth { get; set; } = 16;
        public int AudioSampleRate { get; set; } = 48000;
        public VideoFormats VideoFormat { get; set; } = VideoFormats.H264;
        public VideoRateControls VideoRateControl { get; set; } = VideoRateControls.VBR;
        public int VideoBitRate { get; set; } = 35000;
        public int VideoFrameRate { get; set; } = 60;
    };

    public struct CaptureSettings
    {
        public CaptureSettings() { }
        public int MonitorId { get; set; } = 0;
        public int MaxPixelDimension { get; set; } = 0;
        public bool SoundEffect { get; set; } = true;
        public bool DrawBorder { get; set; } = false;
        public bool DrawMouseCursor { get; set; } = true;
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
        public bool HDRtoSDR { get; set; }
        public float SDRWhiteLevel { get; set; }
        public int PixelByteSize { get; set; }
        public int WidthByteSize { get; set; }
        public int TotalByteSize { get; set; }
    }

    public struct CaptureResult
    {
        public CaptureStatus Status { get; set; }
        public int hResult { get; set; }
        [field: MarshalAs(UnmanagedType.BStr)]
        public string Message { get; set; }
    }
}