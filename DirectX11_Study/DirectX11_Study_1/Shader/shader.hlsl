// 定数バッファ(CPU側からの値受け取り場)
cbuffer global {
	matrix gWVP;    // 変換行列
};

// 頂点情報と色情報を持つ構造体
struct VS_OUTPUT {
	float4 pos : SV_POSITION;
	float4 color : COLOR;
};

// 頂点シェーダ
VS_OUTPUT VS(float4 pos : POSITION, float4 color : COLOR) {
	VS_OUTPUT output;
	output.pos = mul(pos, gWVP);
	output.color = color;
	return output;
}

// ピクセルシェーダ
float4 PS(VS_OUTPUT input) : SV_Target{
	return input.color;
}