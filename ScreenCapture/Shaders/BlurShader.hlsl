float4 TextureBlurGaussian(Texture2D _texture2D, SamplerState _samplerState, float4 color, float2 texCoord, float2 texSize, float blurAmount)
{
    int SampleCount = 1;
    float Pi2 = 6.283185307179586476925286766559F;
    float Directions = 24.0F;
    float Quality = 12.0F;
    float2 Radius = blurAmount / texSize;
    float AddDirection = Pi2 / Directions;
    float AddQuality = 1.0F / Quality;

    for (float d = AddDirection; d <= Pi2; d += AddDirection)
    {
        for (float q = AddQuality; q <= 1.0F; q += AddQuality)
        {
            float2 offset = float2(cos(d), sin(d)) * Radius * q;
            color += _texture2D.Sample(_samplerState, texCoord + offset);
            SampleCount++;
        }
    }

    return color / SampleCount;
}