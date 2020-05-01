#include <Windows.h>

#include <d3d11.h>
#include <d3dcompiler.h>
#include "Functions.h"
#include "Operators.h"
#include "Mesh.h"
#include "Skeleton.h"
#include "Animation.h"
#include "SkinnedMesh.h"
#include "SkinnedMeshShader.h"

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

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hInstancePrev, LPSTR pCmdLine, int nCmdShow)
{
	// Initialize Window
	const LPCSTR WindowName = "GameWindow";
	const int WindowWidth = 640;
	const int WindowHeight = 480;

	WNDCLASS wc{};
	wc.lpfnWndProc = WndProc;
	wc.lpszClassName = WindowName;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	RegisterClass(&wc);

	RECT rect = { 0, 0, WindowWidth, WindowHeight };
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

	HWND hWnd = CreateWindow(WindowName, WindowName, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		rect.right - rect.left, rect.bottom - rect.top,
		NULL, NULL, hInstance, NULL);

	if (hWnd == NULL) return 0;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// Initialize DirectX
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
	scd.OutputWindow = hWnd;
	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;
	scd.Windowed = TRUE;
	D3D_FEATURE_LEVEL fl = D3D_FEATURE_LEVEL_11_0;
	D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &fl, 1,
		D3D11_SDK_VERSION,
		&scd, &pSwapChain, &pDevice, NULL, &pDeviceContext);

	ID3D11RenderTargetView* pBackBufferRenderTargetView;
	ID3D11Texture2D *pbbTex;
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
	dtd.MiscFlags = 0;
	pDevice->CreateTexture2D(&dtd, NULL, &pDepthStencil);
	ID3D11DepthStencilView* pDepthStencilView{};

	pDevice->CreateDepthStencilView(pDepthStencil, NULL, &pDepthStencilView);

	pDeviceContext->OMSetRenderTargets(1, &pBackBufferRenderTargetView, pDepthStencilView);

	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = (FLOAT)WindowWidth;
	vp.Height = (FLOAT)WindowHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	pDeviceContext->RSSetViewports(1, &vp);

	Mesh mesh{ pDevice, pDeviceContext };
	mesh.load("Robot_Kyle.mshs");

	Skeleton skeleton{};
	skeleton.load("Robot_Kyle.skls");

	Animation animation{};
	animation.load("Robot_Kyle.anms");

	SkinnedMesh skinnedMesh{ &mesh, &skeleton, &animation };

	SkinnedMeshShader skinnedMeshShader{ pDevice, pDeviceContext };
	skinnedMeshShader.load("SkinnedMeshShader.hlsl");

	float animTimer{ 0.0f };

	float angle{ 0.0f };

	// Message Loop
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
			XMFLOAT4X4 identity =
			{
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
			};
			skinnedMesh.calculate(identity, animTimer);
			animTimer = std::fmod(animTimer + 1.0f, animation.endFrame());
			angle += 0.01f;

			const float clearColor[4] = { 0.5f, 0.5f, 1.0f, 1.0f };
			pDeviceContext->ClearRenderTargetView(pBackBufferRenderTargetView, clearColor);
			pDeviceContext->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

			XMFLOAT4X4 projection = createPerspectiveFieldOfViewLH(45.0f, 640.0f / 480.0f, 0.1f, 1000.0f);
			XMFLOAT4X4 view = createLookAtLH(XMFLOAT3(0.0f, 10.0f, -30.0f), XMFLOAT3(0.0f, 10.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));

			skinnedMeshShader.projection(projection);
			skinnedMeshShader.view(view);
			skinnedMeshShader.world(createRotationY(angle));
			skinnedMesh.draw(skinnedMeshShader);

			pSwapChain->Present(1, 0);
		}
	}
	pDepthStencilView->Release();
	pDepthStencil->Release();
	pBackBufferRenderTargetView->Release();
	pDeviceContext->Release();
	pDevice->Release();
	pSwapChain->Release();

	return 0;
}