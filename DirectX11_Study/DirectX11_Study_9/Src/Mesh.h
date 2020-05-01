#pragma once

#include <DirectXMath.h>
using namespace DirectX;

#include "Texture2D.h"
#include <vector>
#include <string>
#include <d3d11.h>

class Mesh
{
public:

	struct TempMaterial
	{
		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;
		XMFLOAT4 specular;
		XMFLOAT4 emission;
		float shiniess;
		char textureFileName[128];
		char normalFileName[128];
	};

	struct Material
	{
		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;
		XMFLOAT4 specular;
		XMFLOAT4 emission;
		float shiniess;
		char textureFileName[128];
		char normalFileName[128];
		Texture2D* texture;
		Texture2D* normalTexture;
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
		unsigned char bones[4];
		float weight[4];
		XMFLOAT3 tangent;
		XMFLOAT3 binormal;
	};

	class Shader
	{
	public:

		virtual ~Shader() = default;
		virtual void begin() = 0;
		virtual void end() = 0;
		virtual void material(const Mesh::Material& material) = 0;
	};

public:

	Mesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	~Mesh();

	void load(const std::string& fileName);
	void draw(Mesh::Shader& shader) const;
	void clear();

	Mesh(const Mesh& other) = delete;
	Mesh& operator = (const Mesh& other) = delete;

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