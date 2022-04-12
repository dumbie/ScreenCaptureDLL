Texture2D _texture2D : register(t0);
SamplerState _samplerState : register(s0);
cbuffer _shaderVariables : register(b0)
{
	bool HDRtoSDR;
	float SDRWhiteLevel;
};

struct PS_INPUT
{
	float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD;
};

float4 AdjustSDRWhiteLevel(float4 color)
{
	return color / (SDRWhiteLevel / 70.0F);
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