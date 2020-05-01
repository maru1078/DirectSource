// �萔�o�b�t�@(CPU������̒l�󂯎���)
cbuffer global {
	matrix gWVP;    // �ϊ��s��
};

// ���_���ƐF�������\����
struct VS_OUTPUT {
	float4 pos : SV_POSITION;
	float4 color : COLOR;
};

// ���_�V�F�[�_
VS_OUTPUT VS(float4 pos : POSITION, float4 color : COLOR) {
	VS_OUTPUT output;
	output.pos = mul(pos, gWVP);
	output.color = color;
	return output;
}

// �s�N�Z���V�F�[�_
float4 PS(VS_OUTPUT input) : SV_Target{
	return input.color;
}