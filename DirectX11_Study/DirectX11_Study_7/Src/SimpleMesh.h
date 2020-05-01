#pragma once

#include <DirectXMath.h>
#include <vector>
#include <stack>
#include <d3d11.h>

#include "Texture2D.h"

using namespace DirectX;

class SimpleMesh
{
public:

	struct Material
	{
		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;
		XMFLOAT4 specular;
		XMFLOAT4 emission;
		float shiniess;
		char textureFileName[128];
		Texture2D* texture;
	};

	struct Subset
	{
		int material;
		int start;
		int count;
	};

	struct Vertex
	{
		XMFLOAT3 point;
		XMFLOAT3 normal;
		XMFLOAT2 texcoord;
	};

	class Shader
	{
	public:
		virtual ~Shader() = default;
		virtual void begin() = 0;
		virtual void end() = 0;
		virtual void material(const SimpleMesh::Material& material) = 0;
	};

public:
	SimpleMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	~SimpleMesh();

public:
	void load(const std::string& fileName);
	void draw(SimpleMesh::Shader& shader) const;
	void clear();
	SimpleMesh(const SimpleMesh& other) = delete;
	SimpleMesh& operator = (const SimpleMesh& other) = delete;

private:
	ID3D11Buffer* createBuffer(UINT bind, UINT size, const void* data);

private:
	ID3D11Device* m_device{ nullptr };
	ID3D11DeviceContext* m_deviceContext{ nullptr };
	std::vector<Material> m_materials;
	std::vector<Subset> m_subsets;
	ID3D11Buffer* m_indices{ nullptr };
	ID3D11Buffer* m_vertices{ nullptr };
};
