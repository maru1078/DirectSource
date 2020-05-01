cbuffer ConstantBuffer : register(b0)
{
	float2 texturePosition;
	float2 textureSize;
	float2 size;
	float2 position;
	float2 scale;
	float angle;
	float padding;
	float4 color;
	float2 center;
	float2 screen;
};

struct VS_INPUT
{
	float4 position : POSITION;
};

struct VS_OUTPUT
{
	float2 texcoord : TEXCOORD0;
	float4 position : SV_POSITION;
};

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output;

	output.position = input.position;
	output.position.xy *= size;
	output.position.xy -= center;
	output.position.xy *= scale;

	float2 rotate;
	rotate.x = output.position.x * cos(angle) - output.position.y * sin(angle);
	rotate.y = output.position.x * sin(angle) + output.position.y * cos(angle);

	output.position.xy = rotate;
	output.position.xy += position;
	output.position.xy = output.position.xy * float2(2.0 / screen.x, 2.0 / -screen.y) + float2(-1.0, 1.0);
	output.texcoord.xy = input.position.xy * textureSize + texturePosition;

	return output;
}

Texture2D TextureMapTexture : register(t0);
SamplerState TextureMapSampler : register(s0);

float4 PS(VS_OUTPUT input) : SV_Target
{
	return TextureMapTexture.Sample(TextureMapSampler, input.texcoord) * color;
}