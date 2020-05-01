#pragma once

#include <d3d11.h>

class Sprite2D
{
public:

	class Shader
	{
	public:

		virtual ~Shader() {};

		virtual void begin() = 0;

		virtual void end() = 0;
	};

public:

	Sprite2D(ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	~Sprite2D();

	void draw(Sprite2D::Shader& shader)const;

	Sprite2D(const Sprite2D& other) = delete;
	Sprite2D& operator = (const Shader& other) = delete;

private:

	ID3D11Buffer* createBuffer(UINT bind, UINT size, const void* data);

private:

	ID3D11Device* m_device{ nullptr };
	ID3D11DeviceContext* m_deviceContext{ nullptr };
	ID3D11Buffer* m_indices{ nullptr };
	ID3D11Buffer* m_vertices{ nullptr };
};
