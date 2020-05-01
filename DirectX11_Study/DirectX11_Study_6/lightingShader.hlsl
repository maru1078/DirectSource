cbuffer Constant : register(b0)
{
	matrix WorldMatrix;
	matrix ViewMatrix;
	matrix ProjectionMatrix;
	matrix WorldViewProjMatrix;
	float4 CameraPosition;
	float4 LightPosition;
};

struct VS_INPUT
{
	float4 position : POSITION;
	float3 normal   : NORMAL;
	float2 texcoord : TEXCOORD;
};

struct VS_OUTPUT
{
	float4 color    : COLOR0;
	float2 texcoord : TEXCOORD0;
	float4 position : SV_POSITION;
};

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output;

	float4 worldPosition = mul(WorldMatrix, input.position);
	float3 N = mul((float3x3)WorldMatrix, input.normal);
	float3 L = normalize(LightPosition - worldPosition).xyz;

	float diffuse = max(dot(N, L), 0.0f);

	output.color = diffuse;
	output.texcoord = input.texcoord;
	output.position = mul(WorldViewProjMatrix, input.position);
	return output;
}

Texture2D BaseMapTexture : register(t0);
SamplerState BaseMapSampler : register(s0);

float4 PS(VS_OUTPUT input) : SV_Target
{
	return BaseMapTexture.Sample(BaseMapSampler, input.texcoord) * input.color;
}