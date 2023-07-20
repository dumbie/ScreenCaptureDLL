Texture2D _texture2D : register(t0);
SamplerState _samplerState : register(s0);
cbuffer _shaderVariables : register(b0)
{
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
	if (!HDRtoSDR) { return color; }

	//Adjust SDR white level and HDR paper white
	color = AdjustHDRWhites(color);

	//Adjust HDR maximum nits
	color = AdjustHDRMaximumNits(color);

	//Convert Linear to SRGB
	color = ColorLinearToSrgb(color);

	return color;
}

float GaussianWeight(float2 dist, float sigma)
{
	return exp(-0.5F * dot(dist /= sigma, dist)) / (6.283185307F * sigma * sigma);
}

float4 AdjustBlur(PS_INPUT input)
{
	if (Blur <= 2.0F)
	{
		return _texture2D.Sample(_samplerState, input.TexCoord);
	}

	float4 color;
	float sigma = Blur * 0.25F;
	float resolution = 1.0F / 1024.0F;

	for (int i = 0; i < Blur * Blur; i++)
	{
		float2 dist = float2(i % Blur, i / Blur) - (Blur / 2.0F);
		color += GaussianWeight(dist, sigma) * _texture2D.Sample(_samplerState, input.TexCoord + resolution * dist);
	}

	return color;
}

float4 AdjustSaturation(float4 color)
{
	if (Saturation == 1.0F) { return color; }
	float colorLuminance = (color.r + color.g + color.b) / 3.0F;
	if (Saturation > 1.0F)
	{
		return max(colorLuminance + (color - colorLuminance) * Saturation, color);
	}
	else
	{
		return colorLuminance + (color - colorLuminance) * Saturation;
	}
}

float4 AdjustColorChannels(float4 color)
{
	if (RedChannel == 1.0F && GreenChannel == 1.0F && BlueChannel == 1.0F) { return color; }
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
	if (Brightness == 1.0F) { return color; }
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
	if (Contrast == 0.0F) { return color; }
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
	if (Gamma == 1.0F) { return color; }
	float adjustGamma = Gamma / 1.0F;
	return pow(color, adjustGamma);
}

float4 main(PS_INPUT input) : SV_TARGET
{
	//Get texture colors
	float4 color;

	//Adjust blur
	color = AdjustBlur(input);

	//Get original alpha
	float colorAlpha = color.a;

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

	//Set original alpha
	color.a = colorAlpha;

	//Return updated colors
	return color;
}