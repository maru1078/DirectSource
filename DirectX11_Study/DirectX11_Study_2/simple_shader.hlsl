struct VS_INPUT
{
	float4 position : POSITION;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
};

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output;
	output.position = input.position;
	return output;
}

float4 PS(VS_OUTPUT input) : SV_target
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}