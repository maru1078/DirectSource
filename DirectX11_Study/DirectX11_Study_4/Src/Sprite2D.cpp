#include "Sprite2D.h"

Sprite2D::Sprite2D(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: m_device{ device }
	, m_deviceContext{ deviceContext }
{
	static const unsigned int indices[] =
	{
		0, 1, 2, 3
	};

	static const float vertices[] = 
	{
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f
	};

	m_vertices = createBuffer(D3D11_BIND_VERTEX_BUFFER, sizeof(vertices), vertices);
	m_indices = createBuffer(D3D11_BIND_INDEX_BUFFER, sizeof(indices), indices);
}

Sprite2D::~Sprite2D()
{

}

void Sprite2D::draw(Sprite2D::Shader& shader) const
{
	shader.begin();
	UINT stride = sizeof(float) * 2;
	UINT offset = 0;
	m_deviceContext->IASetVertexBuffers(0, 1, &m_vertices, &stride, &offset);
	m_deviceContext->IASetIndexBuffer(m_indices, DXGI_FORMAT_R32_UINT, 0);
	m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	m_deviceContext->DrawIndexed(4, 0, 0);
	shader.end();
}

ID3D11Buffer* Sprite2D::createBuffer(UINT bind, UINT size, const void* data)
{
	D3D11_BUFFER_DESC desc{};
	desc.ByteWidth = size;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = bind;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA subresourceData{};
	subresourceData.pSysMem = data;

	ID3D11Buffer* buffer{};
	m_device->CreateBuffer(&desc, &subresourceData, &buffer);

	return buffer;
}