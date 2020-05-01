#include "Sprite2DShader.h"

#include <d3dcompiler.h>
#include <memory>

#include <stdexcept>
#include <vector>
#include <algorithm>
#include <stdio.h>

Sprite2DShader::Sprite2DShader(ID3D11Device * device, ID3D11DeviceContext * deviceContext)
	: m_device{ device }
	, m_deviceContext{ deviceContext }
{
	UINT compileFlag = 0;
#ifdef _DEBUG
	compileFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif // _DEBUG

	ID3DBlob* compilePs{ NULL };
	D3DCompileFromFile(L"Sprite2D.hlsl", NULL, NULL, "PS", "ps_5_0", compileFlag, 0, &compilePs, NULL);
	m_device->CreatePixelShader(compilePs->GetBufferPointer(), compilePs->GetBufferSize(), NULL, &m_pixelShader);
	compilePs->Release();

	ID3DBlob* compileVs{ NULL };
	D3DCompileFromFile(L"Sprite2D.hlsl", NULL, NULL, "VS", "vs_5_0", compileFlag, 0, &compileVs, NULL);
	m_device->CreateVertexShader(compileVs->GetBufferPointer(), compileVs->GetBufferSize(), NULL, &m_vertexShader);

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	m_device->CreateInputLayout(layout, 1, compileVs->GetBufferPointer(), compileVs->GetBufferSize(), &m_vertexLayout);
	compileVs->Release();

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.ByteWidth = sizeof(BufferData);
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	m_device->CreateBuffer(&bufferDesc, NULL, &m_constantBuffer);
}

Sprite2DShader::~Sprite2DShader()
{
	m_constantBuffer->Release();
	m_vertexLayout->Release();
	m_pixelShader->Release();
	m_vertexShader->Release();
}

void Sprite2DShader::begin()
{
	m_texture->apply(0);

	D3D11_MAPPED_SUBRESOURCE mappedSubresource{};
	m_deviceContext->Map(m_constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	memcpy_s(mappedSubresource.pData, mappedSubresource.RowPitch, (void*)(&m_data), sizeof(m_data));
	m_deviceContext->Unmap(m_constantBuffer, 0);
	m_deviceContext->VSSetConstantBuffers(0, 1, &m_constantBuffer);
	m_deviceContext->PSSetConstantBuffers(0, 1, &m_constantBuffer);
	m_deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	m_deviceContext->PSSetShader(m_pixelShader, NULL, 0);
	m_deviceContext->IASetInputLayout(m_vertexLayout);
}

void Sprite2DShader::end()
{
}

void Sprite2DShader::screen(const DirectX::XMFLOAT2 & size)
{
	m_data.screen = size;
}

void Sprite2DShader::texture(Texture2D & baseMap)
{
	texture(baseMap, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2{ (float)baseMap.width(), (float)baseMap.height() });
}

void Sprite2DShader::texture(Texture2D & baseMap, const DirectX::XMFLOAT2 & position, const DirectX::XMFLOAT2 & size)
{
	m_texture = &baseMap;
	m_data.size = size;
	m_data.texturePosition = DirectX::XMFLOAT2(position.x / baseMap.width(), position.y / baseMap.height());
	m_data.textureSize = DirectX::XMFLOAT2(size.x / baseMap.width(), size.y / baseMap.height());
}

void Sprite2DShader::translate(const DirectX::XMFLOAT2 & position)
{
	m_data.position = position;
}

void Sprite2DShader::scale(const DirectX::XMFLOAT2 & scale)
{
	m_data.scale = scale;
}

void Sprite2DShader::rotate(float angle)
{
	m_data.angle = angle;
}

void Sprite2DShader::color(const DirectX::XMFLOAT4 color)
{
	m_data.color = color;
}

void Sprite2DShader::center(const DirectX::XMFLOAT2 & center)
{
	m_data.center = center;
}
