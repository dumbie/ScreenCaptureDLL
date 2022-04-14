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
	int HueRotate;
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
	return color / (SDRWhiteLevel / HDRBrightness);
}

float4 AdjustSaturate(float4 color)
{
	float adjustSature = 1.0F - Saturate;
	float adjustLuminance = 1.0 / 3.0;

	float Matrix00 = (adjustSature * adjustLuminance) + Saturate;
	float Matrix10 = adjustSature * adjustLuminance;
	float Matrix20 = adjustSature * adjustLuminance;
	float Matrix01 = adjustSature * adjustLuminance;
	float Matrix11 = (adjustSature * adjustLuminance) + Saturate;
	float Matrix21 = adjustSature * adjustLuminance;
	float Matrix02 = adjustSature * adjustLuminance;
	float Matrix12 = adjustSature * adjustLuminance;
	float Matrix22 = (adjustSature * adjustLuminance) + Saturate;

	float4x4 matrix4x4 =
	{
		Matrix00, Matrix01, Matrix02, 0.0,
		Matrix10, Matrix11, Matrix12, 0.0,
		Matrix20, Matrix21, Matrix22, 0.0,
		0.0, 0.0, 0.0, 1.0F
	};

	return mul(matrix4x4, color);
}

float4 main(PS_INPUT input) : SV_TARGET
{
	//Get texture colors
	float4 color = _texture2D.Sample(_samplerState, input.TexCoord);

	//Apply HDR to SDR tonemapping
	if (HDRtoSDR)
	{
		color = AdjustSDRWhiteLevel(color);
	}

	//Apply Saturate
	color = AdjustSaturate(color);

	//Set alpha channel to maximum
	color.a = 1.0F;

	//Return updated colors
	return color;
}