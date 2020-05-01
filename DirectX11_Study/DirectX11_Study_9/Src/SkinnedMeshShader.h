#pragma once

#include <DirectXMath.h>
#include "SkinnedMesh.h"

using namespace DirectX;

class SkinnedMeshShader
	: public SkinnedMesh::Shader
{
public:

	struct Light
	{
		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;
		XMFLOAT4 specular;
		XMFLOAT3 position;
	};

public:

	SkinnedMeshShader(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	~SkinnedMeshShader();

	void load(const std::string& fileName);
	void world(const XMFLOAT4X4& mat);
	void view(const XMFLOAT4X4& mat);
	void projection(const XMFLOAT4X4& mat);
	void light(const SkinnedMeshShader::Light& light);
	void clear();

	virtual void begin() override;
	virtual void end() override;
	virtual void material(const Mesh::Material& material) override;
	virtual void boneMatrices(int size, const XMFLOAT4X4 matrices[]) override;

	SkinnedMeshShader(const SkinnedMeshShader& other) = delete;
	SkinnedMeshShader& operator = (SkinnedMeshShader& other) = delete;

private:

	ID3D11Device* m_device{ nullptr };
	ID3D11DeviceContext* m_deviceContext{ nullptr };
	ID3D11Buffer* m_constantBuffer{ nullptr };
	ID3D11VertexShader* m_vertexShader{ nullptr };
	ID3D11PixelShader* m_pixelShader{ nullptr };
	ID3D11InputLayout* m_vertexLayout{ nullptr };

	struct BufferData
	{
		XMFLOAT4X4 world;
		XMFLOAT4X4 view;
		XMFLOAT4X4 projection;
		XMFLOAT4X4 boneMatrices[256];

		XMFLOAT4 matAmbient;
		XMFLOAT4 matDiffuse;
		XMFLOAT4 matSpecular;
		XMFLOAT4 matEmissive;
		float matShiniess;
		float pad0[3];

		XMFLOAT4 lightAmbient;
		XMFLOAT4 lightDiffuse;
		XMFLOAT4 lightSpecular;
		XMFLOAT3 lightPosition;
		float pad11[1];
	};

	BufferData m_data;
};