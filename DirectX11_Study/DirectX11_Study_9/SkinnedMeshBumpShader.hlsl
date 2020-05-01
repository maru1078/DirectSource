cbuffer ConstantBuffer : register(b0)
{
	float4x4 WorldMatrix;
	float4x4 ViewMatrix;
	float4x4 ProjectionMatrix;
	float4x4 BoneMatrices[256];

	float4 MatAmbient;
	float4 MatDiffuse;
	float4 MatSpecular;
	float4 MatEmissive;
	float MatShiniess;
	float3 padThree;

	float4 lightAmbient;
	float4 lightDiffuse;
	float4 lightSpecular;
	float3 lightPosition;
	float padOneOne;
};

struct VS_INPUT
{
	float4 Position     : POSITION;
	float3 Normal       : NORMAL;
	float2 TexCoord     : TEXCOORD;
	int4   BlendIndices : BLENDINDICES;
	float4 BlendWeight  : BLENDWEIGHT;
	float3 Tangent      : TANGENT;
	float3 Binormal     : BINORMAL;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD0;
	float3 View     : TEXCOORD1;
	float3 Light    : TEXCOORD2;
};

VS_OUTPUT VS(VS_INPUT Input)
{
	VS_OUTPUT Output;

	// 個の頂点が使用するメッシュのボーンのローカル行列を計算する
	float4x4 LocalMatrix
		= BoneMatrices[Input.BlendIndices.x] * Input.BlendWeight.x
		+ BoneMatrices[Input.BlendIndices.y] * Input.BlendWeight.y
		+ BoneMatrices[Input.BlendIndices.z] * Input.BlendWeight.z
		+ BoneMatrices[Input.BlendIndices.w] * Input.BlendWeight.w;

	// 【この頂点座標の座標変換】
	// ボーンのローカル行列を使って座標変換する
	float4 LocalPosition = mul(LocalMatrix, Input.Position);
	// 定数バッファで取得したワールド行列を使って座標変換する
	float4 WorldPosition = mul(WorldMatrix, LocalPosition);
	// 定数バッファで取得したビュー行列を使って座標変換する
	float4 ViewPosition = mul(ViewMatrix, WorldPosition);

	// 座標変換で使用する4x4の行列を3x3の行列に直す
	float3x3 LocalMatrix3 = (float3x3)LocalMatrix;
	float3x3 WorldMatrix3 = (float3x3)WorldMatrix;
	float3x3 ViewMatrix3 = (float3x3)ViewMatrix;

	// 【この頂点法線の座標変換】（処理は頂点座標の座標変換とほぼ同じ）
	float3 localNormal = mul(LocalMatrix3, Input.Normal);
	float3 worldNormal = mul(WorldMatrix3, localNormal);
	float3 viewNormal = mul(ViewMatrix3, worldNormal);

	// 【この頂点タンジェントの座標変換】（処理は頂点座標の座標変換とほぼ同じ）
	float3 localTangent = mul(LocalMatrix3, Input.Tangent);
	float3 worldTangent = mul(WorldMatrix3, localTangent);
	float3 viewTangent = mul(ViewMatrix3, worldTangent);

	// 【この頂点バイノーマルの座標変換】（処理は頂点座標の座標変換とほぼ同じ）
	float3 localBinormal = mul(LocalMatrix3, Input.Binormal);
	float3 worldBinormal = mul(WorldMatrix3, localBinormal);
	float3 viewBinormal = mul(ViewMatrix3, worldBinormal);

	// ライトの座標をビュー行列を使って座標変換する
	float3 ViewLight = mul(ViewMatrix, float4(lightPosition, 1.0f)).xyz;

	// 接空間変換行列を計算
	float3x3 matTBN = float3x3(normalize(viewTangent), normalize(viewBinormal), normalize(viewNormal));

	// パースペクティブ空間での頂点座標を計算
	Output.Position = mul(ProjectionMatrix, ViewPosition);
	// TexCoordをそのまま渡す
	Output.TexCoord = Input.TexCoord;
	// 頂点→カメラのベクトルを接空間変換行列を使って座標変換
	Output.View = mul(-ViewPosition.xyz, matTBN);
	// 
	Output.Light = mul((ViewLight - ViewPosition.xyz), matTBN);

	return Output;
}

SamplerState g_AlbedoMapSampler : register(s0);
Texture2D    g_AlbedoMapTexture : register(t0);

SamplerState g_NormalMapSampler : register(s1);
Texture2D    g_NormalMapTexture : register(t1);

float4 PS(VS_OUTPUT Input) : SV_TARGET
{
	// ノーマルマップから法線を取得して正規化する
	float3 N = normalize(g_NormalMapTexture.Sample(g_NormalMapSampler, Input.TexCoord).xyz * 2.0 - 1.0);
	// 接空間の頂点→カメラのベクトルを正規化する
	float3 V = normalize(Input.View);
	//
	float3 L = normalize(Input.Light);
	// 
	float3 H = normalize(L + V);

	// 拡散反射光
	float diffuse = max(dot(L, N), 0.0);

	float specular = pow(max(dot(N, H), 0.0), MatShiniess);

	float4 baseColor = g_AlbedoMapTexture.Sample(g_AlbedoMapSampler, Input.TexCoord);

	float4 resultColor
		= MatAmbient * lightAmbient * baseColor
		+ MatDiffuse * lightDiffuse * diffuse * baseColor
		+ MatSpecular * lightSpecular * specular
		+ MatEmissive * baseColor;

	resultColor.a = baseColor.a * MatDiffuse.a;

	return resultColor;
}