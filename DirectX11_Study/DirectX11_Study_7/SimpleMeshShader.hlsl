cbuffer ConstantBuffer : register(b0)
{
	matrix WorldMatrix;
	matrix ViewMatrix;
	matrix ProjectionMatrix;
};

struct VS_INPUT
{
	float4 position : POSITION;
	float3 normal   : NORMAL;
	float2 texcoord : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD0;
};

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output;

	float4 worldPosition = mul(WorldMatrix, input.position);
	float4 viewPosition = mul(ViewMatrix, worldPosition);

	output.position = mul(ProjectionMatrix, viewPosition);
	output.texcoord = input.texcoord;

	return output;
}

SamplerState g_albedoMapSampler : register(s0);
Texture2D g_albedoMatTexture : register(t0);

float4 PS(VS_OUTPUT input) : SV_Target
{
	return g_albedoMatTexture.Sample(g_albedoMapSampler, input.texcoord);
}