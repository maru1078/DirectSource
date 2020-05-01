#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "lodepng.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hInstancePrev, LPSTR mCmdLine, int nCmdShow)
{
	const LPCSTR WindowName = "GameWindow";
	const int WindowWidth = 640;
	const int WindowHeight = 480;

	WNDCLASS wc{};

	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = WindowName;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	RegisterClass(&wc);
	RECT rect{ 0, 0, WindowWidth, WindowHeight };
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

	HWND hwnd = CreateWindow(WindowName, WindowName,
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, hInstance, NULL);

	if (hwnd == NULL) return 0;

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	// Initialize DirectX11
	IDXGISwapChain* pSwapChain;
	ID3D11Device* pDevice;
	ID3D11DeviceContext* pDeviceContext;
	DXGI_SWAP_CHAIN_DESC scd{};
	scd.BufferCount = 1;
	scd.BufferDesc.Width = WindowWidth;
	scd.BufferDesc.Height = WindowHeight;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.RefreshRate.Numerator = 60;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = hwnd;
	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;
	scd.Windowed = TRUE;
	D3D_FEATURE_LEVEL fl = D3D_FEATURE_LEVEL_11_0;
	D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &fl, 1,
		D3D11_SDK_VERSION,
		&scd, &pSwapChain, &pDevice, NULL, &pDeviceContext);

	ID3D11RenderTargetView* pBackBufferRenderTargetView;
	ID3D11Texture2D* pbbTex;
	pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pbbTex);
	pDevice->CreateRenderTargetView(pbbTex, NULL, &pBackBufferRenderTargetView);
	pbbTex->Release();

	ID3D11Texture2D* pDepthStencil;
	D3D11_TEXTURE2D_DESC dtd{};
	dtd.Width = WindowWidth;
	dtd.Height = WindowHeight;
	dtd.MipLevels = 1;
	dtd.ArraySize = 1;
	dtd.Format = DXGI_FORMAT_D32_FLOAT;
	dtd.SampleDesc.Count = 1;
	dtd.SampleDesc.Quality = 0;
	dtd.Usage = D3D11_USAGE_DEFAULT;
	dtd.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	dtd.CPUAccessFlags = 0;
	dtd.MipLevels = 0;
	pDevice->CreateTexture2D(&dtd, NULL, &pDepthStencil);
	ID3D11DepthStencilView* pDepthStencilView{};

	pDevice->CreateDepthStencilView(pDepthStencil, NULL, &pDepthStencilView);

	pDeviceContext->OMSetRenderTargets(1, &pBackBufferRenderTargetView, pDepthStencilView);

	const float vertices[]
	{
		-1.0f,  1.0f, 0.0f, 0.0f, 0.0f,
		 1.0f,  1.0f, 0.0f, 1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f, 1.0f, 1.0f,
	    -1.0f, -1.0f, 0.0f, 0.0f, 1.0f
	};

	ID3D11Buffer* pVertexBuffer;
	D3D11_BUFFER_DESC vbd{};
	vbd.ByteWidth = sizeof(vertices);
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA dataVertex{};
	dataVertex.pSysMem = vertices;
	pDevice->CreateBuffer(&vbd, &dataVertex, &pVertexBuffer);

	const unsigned int indices[]
	{   
		0, 1, 2,
		0, 2, 3 
	};

	ID3D11Buffer* pIndexBuffer;
	D3D11_BUFFER_DESC ibd{};
	ibd.ByteWidth = sizeof(indices);
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA dataIndex{};
	dataIndex.pSysMem = indices;
	pDevice->CreateBuffer(&ibd, &dataIndex, &pIndexBuffer);

	UINT shaderFlag = 0;
#ifdef _DEBUG
	shaderFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif // _DEBUG

	ID3D11VertexShader* pVertexShader;
	ID3DBlob* pCompileVS{};
	D3DCompileFromFile(L"textureShader.hlsl", NULL, NULL, "VS", "vs_5_0", shaderFlag, 0, &pCompileVS, NULL);
	pDevice->CreateVertexShader(pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(),
		NULL, &pVertexShader);

	ID3D11PixelShader* pPixelShader;
	ID3DBlob* pCompilePS{};
	D3DCompileFromFile(L"textureShader.hlsl", NULL, NULL, "PS", "ps_5_0", shaderFlag, 0, &pCompilePS, NULL);
	pDevice->CreatePixelShader(pCompilePS->GetBufferPointer(), pCompilePS->GetBufferSize(),
		NULL, &pPixelShader);
	pCompilePS->Release();

	ID3D11InputLayout* pVertexLayout;
	D3D11_INPUT_ELEMENT_DESC layout[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	pDevice->CreateInputLayout(layout, 2,
		pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), &pVertexLayout);
	pCompileVS->Release();

	struct ConstantBuffer
	{
		DirectX::XMMATRIX WorldViewProjMatrix;
	};
	ID3D11Buffer* pConstantBuffer;
	D3D11_BUFFER_DESC cbd;
	cbd.ByteWidth = sizeof(ConstantBuffer);
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.MiscFlags = 0;
	cbd.StructureByteStride = 0;
	pDevice->CreateBuffer(&cbd, NULL, &pConstantBuffer);

	unsigned int width{ 0 };
	unsigned int height{ 0 };
	std::vector<unsigned char> image;
	lodepng::decode(image, width, height, "test.png");

	D3D11_TEXTURE2D_DESC t2d{};
	t2d.Width = width;
	t2d.Height = height;
	t2d.MipLevels = 1;
	t2d.ArraySize = 1;
	t2d.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	t2d.SampleDesc.Count = 1;
	t2d.SampleDesc.Quality = 0;
	t2d.Usage = D3D11_USAGE_DEFAULT;
	t2d.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	t2d.CPUAccessFlags = 0;
	t2d.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA dataTex2d{};
	dataTex2d.pSysMem = image.data();
	dataTex2d.SysMemPitch = width * 4;
	dataTex2d.SysMemSlicePitch = width * height * 4;

	ID3D11Texture2D* pTexture2D{};
	pDevice->CreateTexture2D(&t2d, &dataTex2d, &pTexture2D);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvd{};
	srvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MipLevels = 1;
	ID3D11ShaderResourceView* pTextureShaderResourceView;
	pDevice->CreateShaderResourceView(pTexture2D, &srvd, &pTextureShaderResourceView);

	D3D11_SAMPLER_DESC spd{};
	spd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	spd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	spd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	spd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	spd.MipLODBias = 0.0f;
	spd.MaxAnisotropy = 1;
	spd.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	spd.BorderColor[0] = 0;
	spd.BorderColor[1] = 0;
	spd.BorderColor[2] = 0;
	spd.BorderColor[3] = 0;
	spd.MinLOD = 0;
	spd.MaxLOD = D3D11_FLOAT32_MAX;

	ID3D11SamplerState* pSamplerState{};
	pDevice->CreateSamplerState(&spd, &pSamplerState);

	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = (FLOAT)WindowWidth;
	vp.Height = (FLOAT)WindowHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	pDeviceContext->RSSetViewports(1, &vp);

	float angle{ 0.0f };

	MSG msg{};
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			angle += 0.01f;

			float clearColor[4] = { 0.5f, 0.5f, 1.0f, 1.0f };
			pDeviceContext->ClearRenderTargetView(pBackBufferRenderTargetView, clearColor);
			pDeviceContext->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

			DirectX::XMMATRIX projection = DirectX::XMMatrixPerspectiveFovLH(
				DirectX::XM_PIDIV4,
				640.0f / 480.0f,
				0.1f,
				1000.0f
			);

			DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(
				{ 0.0f, 0.0f, -5.0f },
				{ 0.0f, 0.0f,  0.0f },
				{ 0.0f, 1.0f,  0.0f }
			);

			DirectX::XMMATRIX world = DirectX::XMMatrixRotationY(angle);

			ConstantBuffer cbf;
			cbf.WorldViewProjMatrix = world * view * projection;
			D3D11_MAPPED_SUBRESOURCE mappedSubResource{};
			pDeviceContext->Map(pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource);

			memcpy_s(mappedSubResource.pData, mappedSubResource.RowPitch,
				(void*)&cbf, sizeof(cbf));
			pDeviceContext->Unmap(pConstantBuffer, 0);

			pDeviceContext->VSSetConstantBuffers(0, 1, &pConstantBuffer);
			pDeviceContext->PSSetConstantBuffers(0, 1, &pConstantBuffer);

			pDeviceContext->VSSetShader(pVertexShader, NULL, 0);
			pDeviceContext->PSSetShader(pPixelShader, NULL, 0);
			pDeviceContext->IASetInputLayout(pVertexLayout);

			pDeviceContext->PSSetShaderResources(0, 1, &pTextureShaderResourceView);
			pDeviceContext->PSSetSamplers(0, 1, &pSamplerState);

			UINT stride = sizeof(float) * 5;
			UINT offset = 0;
			pDeviceContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
			pDeviceContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
			pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			pDeviceContext->DrawIndexed(6, 0, 0);

			pTextureShaderResourceView;

			pSwapChain->Present(1, 0);
		}
	}
	pSamplerState->Release();
	//pTextureShaderResourceView->Release();
	//pTexture2D->Release();
	pConstantBuffer->Release();
	pVertexLayout->Release();
	pPixelShader->Release();
	pVertexShader->Release();
	pVertexBuffer->Release();
	pIndexBuffer->Release();
	pDepthStencilView->Release();
	pDepthStencil->Release();
	pBackBufferRenderTargetView->Release();
	pDeviceContext->Release();
	pDevice->Release();
	pSwapChain->Release();

	return 0;
}