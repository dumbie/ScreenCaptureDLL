Texture2D _texture2D : register(t0);
SamplerState _samplerState : register(s0);
cbuffer _shaderVariables : register(b0)
{
	bool HDRtoSDR;
	float HDRBrightness;
	float SDRWhiteLevel;
	float Vibrance;
	float Saturate;
	float Temperature;
	float RedChannel;
	float GreenChannel;
	float BlueChannel;
	float Brightness;
	float Contrast;
	float Gamma;
};

struct PS_INPUT
{
	float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD;
};

float4 AdjustSDRWhiteLevel(float4 color)
{
	if (!HDRtoSDR) { return color; }
	return color / (SDRWhiteLevel / HDRBrightness);
}

float4 AdjustVibrance(float4 color)
{
	if (Vibrance == 0.0F) { return color; }
	float maxColor = max(color.r, max(color.g, color.b));
	float minColor = min(color.r, min(color.g, color.b));
	float4 lerpColor = lerp(minColor, color, 1.0F + (1.0F - pow(maxColor, Vibrance)));
	return min(max(lerpColor, 0), 255);
}

float4 AdjustSaturate(float4 color)
{
	if (Saturate == 1.0F) { return color; }
	float adjustSaturate = 1.0F - Saturate;
	float adjustLuminance = (color.r + color.g + color.b) / 3.0F;
	return color * Saturate + adjustSaturate * adjustLuminance;
}

float4 AdjustTemperature(float4 color)
{
	if (Temperature == 0.0F) { return color; }

	float redTemp = 255;
	if (Temperature > 66)
	{
		redTemp = Temperature - 60;
		redTemp = 329.698727466 * pow(redTemp, -0.1332047592);
	}

	float greenTemp = 0;
	if (Temperature <= 66)
	{
		greenTemp = Temperature;
		greenTemp = (99.4708025861 * log(greenTemp)) - 161.1195681661;
	}
	else
	{
		greenTemp = Temperature - 60;
		greenTemp = 288.1221695283 * pow(greenTemp, -0.0755148492);
	}

	float blueTemp = 255;
	if (Temperature < 66)
	{
		if (Temperature <= 19)
		{
			blueTemp = 0;
		}
		else
		{
			blueTemp = Temperature - 10;
			blueTemp = (138.5177312231 * log(blueTemp)) - 305.0447927307;
		}
	}

	float4x4 matrix4x4 =
	{
		redTemp / 255.0F, 0.0, 0.0, 0.0,
		0.0, greenTemp / 255.0F, 0.0, 0.0,
		0.0, 0.0, blueTemp / 255.0F, 0.0,
		0.0, 0.0, 0.0, 1.0F
	};
	return mul(matrix4x4, color);
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
	if (Gamma == 1.1F) { return color; }
	float adjustGamma = Gamma / 1.0F;
	return pow(color, adjustGamma);
}

float4 main(PS_INPUT input) : SV_TARGET
{
	//Get texture colors
	float4 color = _texture2D.Sample(_samplerState, input.TexCoord);

	//Adjust SDR white level
	color = AdjustSDRWhiteLevel(color);

	//Adjust vibrance
	color = AdjustVibrance(color);

	//Adjust saturate
	color = AdjustSaturate(color);

	//Adjust temperature
	color = AdjustTemperature(color);

	//Adjust color channels
	color = AdjustColorChannels(color);

	//Adjust brightness
	color = AdjustBrightness(color);

	//Adjust contrast
	color = AdjustContrast(color);

	//Adjust gamma
	color = AdjustGamma(color);

	//Set alpha channel to maximum
	color.a = 1.0F;

	//Return updated colors
	return color;
}