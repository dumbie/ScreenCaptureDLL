Texture2D _texture2D : register(t0);
SamplerState _samplerState : register(s0);
cbuffer _shaderVariables : register(b0)
{
	bool TextureFilterUse;
	bool HDRtoSDR;
	float HDRPaperWhite;
	float HDRMaximumNits;
	float SDRWhiteLevel;
	float Saturation;
	float RedChannel;
	float GreenChannel;
	float BlueChannel;
	float Brightness;
	float Contrast;
	float Gamma;
	float Blur;
	float Unused1;
	float Unused2;
	float Unused3;
};

struct PS_INPUT
{
	float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD;
};

float4 ColorSrgbToLinear(float4 color)
{
	float3 sRGB = color.rgb;
	float3 sRGBNew = sRGB * (sRGB * (sRGB * 0.305306011 + 0.682171111) + 0.012522878);
	return float4(sRGBNew, color.a);
}

float4 ColorLinearToSrgb(float4 color)
{
	float3 RGB = color.rgb;
	float3 S1 = sqrt(RGB);
	float3 S2 = sqrt(S1);
	float3 S3 = sqrt(S2);
	float3 RGBNew = 0.662002687 * S1 + 0.684122060 * S2 - 0.323583601 * S3 - 0.0225411470 * RGB;
	return float4(RGBNew, color.a);
}

float ColorLinearToST2084(float color)
{
	float colorPow = pow(abs(color), 0.1593017578F);
	return pow((0.8359375F + 18.8515625F * colorPow) / (1.0F + 18.6875F * colorPow), 78.84375F);
}

float ColorST2084ToLinear(float color)
{
	float colorPow = pow(abs(color), 1.0F / 78.84375F);
	return pow(abs(max(colorPow - 0.8359375F, 0.0F) / (18.8515625F - 18.6875F * colorPow)), 1.0F / 0.1593017578F);
}

float4 AdjustHDRMaximumNits(float4 color)
{
	float colorMax = max(max(color.r, color.g), color.b);
	float color2084 = ColorLinearToST2084(colorMax);

	float nits2084 = ColorLinearToST2084(HDRMaximumNits / 1000.0F);
	nits2084 = saturate(color2084 / nits2084) * nits2084;
	nits2084 = ColorST2084ToLinear(nits2084);

	return color *= nits2084 / colorMax;
}

float4 AdjustHDRWhites(float4 color)
{
	return color /= SDRWhiteLevel / HDRPaperWhite;
}

float4 AdjustHDRtoSDR(float4 color)
{
	if (!HDRtoSDR)
	{
		return color;
	}

	//Adjust SDR white level and HDR paper white
	color = AdjustHDRWhites(color);

	//Adjust HDR maximum nits
	color = AdjustHDRMaximumNits(color);

	//Convert Linear to SRGB
	color = ColorLinearToSrgb(color);

	return color;
}

float4 AdjustTextureBlur(float4 color, float2 texCoord, float2 texSize)
{
	if (Blur < 2.0F)
	{
		return color;
	}

	int SampleCount = 1;
	float Pi2 = 6.283185307179586476925286766559F;
	float Directions = 24.0F;
	float Quality = 12.0F;
	float2 Radius = Blur / texSize;
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

float4 AdjustTextureFilter(float4 color, float2 texCoord, float2 ddxy)
{
	if (!TextureFilterUse || Blur >= 2.0F)
	{
		return color;
	}

	int sampleCount = 9;
	float sharpness = 0.75F;
	float2 dir1 = ddxy * 0.0625F * sharpness;
	float2 dir3 = ddxy * 0.1875F * sharpness;
	float2 dir5 = ddxy * 0.3125F * sharpness;
	float2 dir7 = ddxy * 0.4375F * sharpness;
	color += _texture2D.Sample(_samplerState, texCoord + float2(dir1.x, -dir3.y));
	color += _texture2D.Sample(_samplerState, texCoord + float2(-dir1.x, dir3.y));
	color += _texture2D.Sample(_samplerState, texCoord + float2(dir5.x, dir1.y));
	color += _texture2D.Sample(_samplerState, texCoord + float2(-dir3.x, -dir5.y));
	color += _texture2D.Sample(_samplerState, texCoord + float2(-dir5.x, dir5.y));
	color += _texture2D.Sample(_samplerState, texCoord + float2(-dir7.x, -dir1.y));
	color += _texture2D.Sample(_samplerState, texCoord + float2(dir3.x, dir7.y));
	color += _texture2D.Sample(_samplerState, texCoord + float2(dir7.x, -dir7.y));
	return color / sampleCount;
}

float4 AdjustSaturation(float4 color)
{
	if (Saturation == 1.0F)
	{
		return color;
	}
	float colorLuminance = (color.r + color.g + color.b) / 3.0F;
	return colorLuminance + (color - colorLuminance) * Saturation;
}

float4 AdjustColorChannels(float4 color)
{
	if (RedChannel == 1.0F && GreenChannel == 1.0F && BlueChannel == 1.0F)
	{
		return color;
	}
	float4x4 matrix4x4 =
	{
		RedChannel, 0.0, 0.0, 0.0,
		0.0, GreenChannel, 0.0, 0.0,
		0.0, 0.0, BlueChannel, 0.0,
		0.0, 0.0, 0.0, 1.0F
	};
	return mul(matrix4x4, color);
}

float4 AdjustBrightness(float4 color)
{
	if (Brightness == 1.0F)
	{
		return color;
	}
	float4x4 matrix4x4 =
	{
		Brightness, 0.0, 0.0, 0.0,
		0.0, Brightness, 0.0, 0.0,
		0.0, 0.0, Brightness, 0.0,
		0.0, 0.0, 0.0, 1.0F
	};
	return mul(matrix4x4, color);
}

float4 AdjustContrast(float4 color)
{
	if (Contrast == 0.0F)
	{
		return color;
	}
	float4x4 matrix4x4 =
	{
		1.0F, 0.0, 0.0, Contrast,
		0.0, 1.0F, 0.0, Contrast,
		0.0, 0.0, 1.0F, Contrast,
		0.0, 0.0, 0.0, 1.0F
	};
	return mul(matrix4x4, color);
}

float4 AdjustGamma(float4 color)
{
	if (Gamma == 1.0F)
	{
		return color;
	}
	float adjustGamma = Gamma / 1.0F;
	return pow(color, adjustGamma);
}

float4 main(PS_INPUT input) : SV_TARGET
{
	//Get texture colors
	float4 color = _texture2D.Sample(_samplerState, input.TexCoord);

	//Get original texture alpha
	float colorAlpha = color.a;

	//Get texture size
	float2 textureSize = (0.0F, 0.0F);
	_texture2D.GetDimensions(textureSize.x, textureSize.y);

	//Get texture ddxy
	float2 textureDdxy = (ddx(input.TexCoord.x), ddy(input.TexCoord.y));

	//Adjust texture filter
	color = AdjustTextureFilter(color, input.TexCoord, textureDdxy);

	//Adjust texture blur
	color = AdjustTextureBlur(color, input.TexCoord, textureSize);

	//Adjust HDR to SDR
	color = AdjustHDRtoSDR(color);

	//Adjust saturation
	color = AdjustSaturation(color);

	//Adjust color channels
	color = AdjustColorChannels(color);

	//Adjust brightness
	color = AdjustBrightness(color);

	//Adjust contrast
	color = AdjustContrast(color);

	//Adjust gamma
	color = AdjustGamma(color);

	//Set original texture alpha
	color.a = colorAlpha;

	//Return updated colors
	return color;
}