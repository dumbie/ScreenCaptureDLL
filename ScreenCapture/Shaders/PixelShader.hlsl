Texture2D _texture2D : register(t0);
SamplerState _samplerState : register(s0);
cbuffer _shaderVariables : register(b0)
{
	bool HDRtoSDR;
	float HDRBrightness;
	float SDRWhiteLevel;
	float Saturation;
	float Temperature;
	float HueRotate;
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

float4 main(PS_INPUT input) : SV_TARGET
{
	//Get texture colors
	float4 color = _texture2D.Sample(_samplerState, input.TexCoord);

	//Apply HDR to SDR tonemapping
	if (HDRtoSDR)
	{
		color = AdjustSDRWhiteLevel(color);
	}

	//Set alpha channel to maximum
	color.a = 1.0F;

	//Return updated colors
	return color;
}