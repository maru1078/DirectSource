#pragma once

#include <DirectXMath.h>
#include "Sprite2D.h"
#include "Texture2D.h"

class Sprite2DShader
	: public Sprite2D::Shader
{
public:

	explicit Sprite2DShader(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	~Sprite2DShader();

	virtual void begin();
	virtual void end();

	void screen(const DirectX::XMFLOAT2& size);
	void texture(Texture2D& baseMap);
	void texture(Texture2D& baseMap, const DirectX::XMFLOAT2& position, const DirectX::XMFLOAT2& size);
	void translate(const DirectX::XMFLOAT2& position);
	void scale(const DirectX::XMFLOAT2& scale);
	void rotate(float angle);
	void color(const DirectX::XMFLOAT4 color);
	void center(const DirectX::XMFLOAT2& center);

	Sprite2DShader(const Sprite2DShader& other) = delete;
	Sprite2DShader& operator = (const Sprite2DShader& other) = delete;

private:
	ID3D11Device* m_device{ nullptr };
	ID3D11DeviceContext* m_deviceContext{ nullptr };
	ID3D11Buffer* m_constantBuffer{ nullptr };
	ID3D11VertexShader* m_vertexShader{ nullptr };
	ID3D11PixelShader* m_pixelShader{ nullptr };
	ID3D11InputLayout* m_vertexLayout{ nullptr };
	Texture2D* m_texture{ nullptr };

	struct BufferData
	{
		DirectX::XMFLOAT2 texturePosition{ 0.0f, 0.0f };
		DirectX::XMFLOAT2 textureSize{ 0.0f, 0.0f };
		DirectX::XMFLOAT2 size{ 0.0f, 0.0f };
		DirectX::XMFLOAT2 position{ 0.0f, 0.0f };
		DirectX::XMFLOAT2 scale{ 1.0f, 1.0f };
		float angle{ 0.0f };
		float padding{ 0.0f };
		DirectX::XMFLOAT4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
		DirectX::XMFLOAT2 center{ 0.0f, 0.0f };
		DirectX::XMFLOAT2 screen{ 640.0f, 480.0f };
	};

	BufferData m_data{};
};