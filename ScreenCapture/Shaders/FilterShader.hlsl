//Box filter

//Bicubic filter

//Catmull-Rom filter

//Mitchell-Netravali filter

//Blackman-Harris filter

//Spline filter

//Hermite filter

//Hanning filter

//Hamming filter

//Lanczos filter

//Bilinear filter

//Multisample anti-aliasing filter
float4 TextureFilterMsaa(Texture2D _texture2D, SamplerState _samplerState, float2 texCoord, float2 texDdxy)
{
    //float2 dir0 = texDdxy * 0.0000F;
    float2 dir1 = texDdxy * 0.0625F;
    //float2 dir2 = texDdxy * 0.1250F;
    float2 dir3 = texDdxy * 0.1875F;
    //float2 dir4 = texDdxy * 0.2500F;
    float2 dir5 = texDdxy * 0.3125F;
    //float2 dir6 = texDdxy * 0.3750F;
    float2 dir7 = texDdxy * 0.4375F;
    //float2 dir8 = texDdxy * 0.5000F;

    //2x
    //int sampleCount = 3;
    //color += _texture2D.Sample(_samplerState, texCoord + float2(dir4.x, dir4.y));
    //color += _texture2D.Sample(_samplerState, texCoord + float2(-dir4.x, -dir4.y));
    //return color / sampleCount;

	//4x
    //int sampleCount = 5;
    //color += _texture2D.Sample(_samplerState, texCoord + float2(-dir2.x, -dir6.y));
    //color += _texture2D.Sample(_samplerState, texCoord + float2(dir6.x, -dir2.y));
    //color += _texture2D.Sample(_samplerState, texCoord + float2(-dir6.x, dir2.y));
    //color += _texture2D.Sample(_samplerState, texCoord + float2(dir2.x, dir6.y));
    //return color / sampleCount;

	//8x
    int sampleCount = 8;
    float4 color = _texture2D.Sample(_samplerState, texCoord + float2(dir1.x, -dir3.y));
    color += _texture2D.Sample(_samplerState, texCoord + float2(-dir1.x, dir3.y));
    color += _texture2D.Sample(_samplerState, texCoord + float2(dir5.x, dir1.y));
    color += _texture2D.Sample(_samplerState, texCoord + float2(-dir3.x, -dir5.y));
    color += _texture2D.Sample(_samplerState, texCoord + float2(-dir5.x, dir5.y));
    color += _texture2D.Sample(_samplerState, texCoord + float2(-dir7.x, -dir1.y));
    color += _texture2D.Sample(_samplerState, texCoord + float2(dir3.x, dir7.y));
    color += _texture2D.Sample(_samplerState, texCoord + float2(dir7.x, -dir7.y));
    return color / sampleCount;

	//16x
    //int sampleCount = 17;
    //color += _texture2D.Sample(_samplerState, texCoord + float2(dir1.x, dir1.y));
    //color += _texture2D.Sample(_samplerState, texCoord + float2(-dir1.x, -dir3.y));
    //color += _texture2D.Sample(_samplerState, texCoord + float2(-dir3.x, dir2.y));
    //color += _texture2D.Sample(_samplerState, texCoord + float2(dir4.x, -dir1.y));
    //color += _texture2D.Sample(_samplerState, texCoord + float2(-dir5.x, -dir2.y));
    //color += _texture2D.Sample(_samplerState, texCoord + float2(dir2.x, dir5.y));
    //color += _texture2D.Sample(_samplerState, texCoord + float2(dir5.x, dir3.y));
    //color += _texture2D.Sample(_samplerState, texCoord + float2(dir3.x, -dir5.y));
    //color += _texture2D.Sample(_samplerState, texCoord + float2(-dir2.x, dir6.y));
    //color += _texture2D.Sample(_samplerState, texCoord + float2(dir0.x, -dir7.y));
    //color += _texture2D.Sample(_samplerState, texCoord + float2(-dir4.x, -dir6.y));
    //color += _texture2D.Sample(_samplerState, texCoord + float2(-dir6.x, dir4.y));
    //color += _texture2D.Sample(_samplerState, texCoord + float2(-dir8.x, dir0.y));
    //color += _texture2D.Sample(_samplerState, texCoord + float2(dir7.x, -dir4.y));
    //color += _texture2D.Sample(_samplerState, texCoord + float2(dir6.x, dir7.y));
    //color += _texture2D.Sample(_samplerState, texCoord + float2(-dir7.x, -dir8.y));
    //return color / sampleCount;
}