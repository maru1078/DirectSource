cbuffer ConstantBuffer : register(b0)
{
	matrix WorldViewProjMatrix;
};

struct VS_INPUT
{
	float4 position : POSITION;
	float2 texcoord : TEXCOORD0;
};

struct VS_OUTPUT
{
	float2 texcoord : TEXCOORD0;
	float4 position : SV_POSITION;
};

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output;

	output.texcoord = input.texcoord;
	output.position = mul(WorldViewProjMatrix, input.position);
	return output;
}

Texture2D BaseMapTexture : register(t0);
SamplerState BaseMapSampler : register(s0);

float4 PS(VS_OUTPUT input) : SV_Target
{
	return BaseMapTexture.Sample(BaseMapSampler, input.texcoord);
}