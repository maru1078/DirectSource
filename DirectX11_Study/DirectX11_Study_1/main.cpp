/*

個人的にサイトを見ていろいろ試してみたやつ

*/


#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")  // デバイスとスワップチェーンの作成に必要

#include <d3dcompiler.h> // シェーダーのコンパイルに必要
#pragma comment(lib, "d3dcompiler.lib")

#include <DirectXMath.h>
#include <Windows.h>

using namespace DirectX;

// 一つの頂点情報を格納する構造体
struct VERTEX {
	XMFLOAT3 V;
	XMFLOAT4 C;
};

// GPU(シェーダ側)へ送る数値をまとめた構造体
struct CONSTANT_BUFFER {
	XMMATRIX mWVP;
};


#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 540

int clientWidth;
int clientHeight;

ID3D11Device*           pDevice;           // デバイス
IDXGISwapChain*         pSwapChain;        // スワップチェーン
ID3D11DeviceContext*    pDeviceContext;    // デバイスコンテキスト
ID3D11RenderTargetView* pRenderTargetView; // レンダーターゲットビュー
ID3D11InputLayout*      pInputLayout;      // インプットレイアウト
ID3D11Buffer*           pConstantBuffer;   // 定数バッファ
ID3D11Buffer*           pVertexBuffer;     // 頂点バッファ
ID3D11RasterizerState*  pRasterizerState;  // ラスタライザーステイト
ID3D11VertexShader*     pVertexShader;     // 頂点シェーダー
ID3D11PixelShader*      pPixelShader;      // ピクセルシェーダー

LRESULT CALLBACK WinProc(HWND hWnd, UINT uMsg, WPARAM wparam, LPARAM lparam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
		// プログラムを終了させるメッセージ
		PostQuitMessage(0);
		return 0;

	case WM_CREATE:
	{
		// パイプラインの準備
		RECT cSize;
		GetClientRect(hWnd, &cSize);
		clientWidth = cSize.right;
		clientHeight = cSize.bottom;

		DXGI_SWAP_CHAIN_DESC scd = { 0 };
		scd.BufferCount = 1;
		scd.BufferDesc.Width = clientWidth;
		scd.BufferDesc.Height = clientHeight;
		scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		scd.BufferDesc.RefreshRate.Numerator = 60;
		scd.BufferDesc.RefreshRate.Denominator = 1;
		scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scd.OutputWindow = hWnd;
		scd.SampleDesc.Count = 1;
		scd.SampleDesc.Quality = 0;
		scd.Windowed = TRUE;

		D3D_FEATURE_LEVEL fl = D3D_FEATURE_LEVEL_11_0;

		// デバイスとスワップチェーンの作成
		D3D11CreateDeviceAndSwapChain(
			NULL,
			D3D_DRIVER_TYPE_HARDWARE, // ドライバータイプ
			NULL,
			0,
			&fl,                      // フィーチャーレベル
			1,
			D3D11_SDK_VERSION,        // SDKのバージョン
			&scd,                     // スワップチェーンデスク
			&pSwapChain,              // スワップチェーン
			&pDevice,                 // デバイス
			NULL,
			&pDeviceContext           // デバイスコンテキスト
		);

		ID3D11Texture2D *pbbTex;
		pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pbbTex);
		// レンダーターゲットの作成
		pDevice->CreateRenderTargetView(
			pbbTex,
			NULL,
			&pRenderTargetView // レンダーターゲットビュー
		);
		pbbTex->Release();

		/*

		ここまで設定すると、ウィンドウの背景色の指定が可能。

		*/

		// ビューポートの設定
		D3D11_VIEWPORT vp;
		vp.Width = clientWidth;
		vp.Height = clientHeight;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;

		// シェーダーの設定
		ID3DBlob* pCompileVS; // 頂点シェーダー
		ID3DBlob* pCompilePS; // ピクセルシェーダー
		// 頂点シェーダーのコンパイル
		D3DCompileFromFile(
			L"Shader/shader.hlsl", // ファイルパス
			NULL,
			NULL,
			"VS", // エントリーポイント
			"vs_5_0",
			NULL,
			0,
			&pCompileVS,
			NULL
		);

		// シェーダー作成
		pDevice->CreateVertexShader(pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), NULL, &pVertexShader);

		// ピクセルシェーダーのコンパイル
		D3DCompileFromFile(
			L"Shader/shader.hlsl",
			NULL,
			NULL,
			"PS",
			"ps_5_0",
			NULL,
			0,
			&pCompilePS,
			NULL
		);

		// シェーダー作成
		pDevice->CreatePixelShader(pCompilePS->GetBufferPointer(), pCompilePS->GetBufferSize(), NULL, &pPixelShader);

		// 頂点レイアウト
		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0  },
		    { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 } // 頂点ごとに色を付ける際に追加した
		};

		pDevice->CreateInputLayout(
			layout,
			2, // レイアウトにある情報（位置、色）の数。位置と色がある場合「2」
			pCompileVS->GetBufferPointer(),
			pCompileVS->GetBufferSize(),
			&pInputLayout
		);
		pCompileVS->Release();
		pCompilePS->Release();

		// 定数バッファの設定
		D3D11_BUFFER_DESC cb;
		cb.ByteWidth = sizeof(CONSTANT_BUFFER);
		cb.Usage = D3D11_USAGE_DYNAMIC;
		cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cb.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		cb.MiscFlags = 0;
		cb.StructureByteStride = 0;
		pDevice->CreateBuffer(&cb, NULL, &pConstantBuffer);

		// 頂点座標
		VERTEX vertices[] =
		{
			{ XMFLOAT3(0.0f, 0.5f, 2.0f),   XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
			{ XMFLOAT3(-0.5f, 0.0f, 2.0f),  XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)},
			{ XMFLOAT3(0.5f, 0.0f, 2.0f),   XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)},
			{ XMFLOAT3(0.0f, -0.5f, 2.0f),  XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)}
		};

		D3D11_BUFFER_DESC bd;
		bd.ByteWidth = sizeof(VERTEX) * 4;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = vertices;
		pDevice->CreateBuffer(&bd, &initData, &pVertexBuffer);

		// ラスタライザの設定
		D3D11_RASTERIZER_DESC rd = {};
		rd.FillMode = D3D11_FILL_SOLID; // D3D11_FILL_WIREFRAMEでワイヤーフレーム、D3D11_FILL_SOLID
		rd.CullMode = D3D11_CULL_NONE;
		rd.FrontCounterClockwise = TRUE;
		pDevice->CreateRasterizerState(&rd, &pRasterizerState);

		// パイプライン構築
		UINT stride = sizeof(VERTEX);
		UINT offset = 0;
		pDeviceContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);     // 頂点データをセット
		pDeviceContext->IASetInputLayout(pInputLayout);                                 // 頂点レイアウトをセット
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP); // データの入力種類を指定
		pDeviceContext->OMSetRenderTargets(1, &pRenderTargetView, NULL);                // レンダーターゲットビューのセット
		pDeviceContext->RSSetViewports(1, &vp);                                         // ビューポートのセット
		pDeviceContext->VSSetShader(pVertexShader, NULL, 0);                            // 頂点シェーダをセット
		pDeviceContext->PSSetShader(pPixelShader, NULL, 0);                             // ピクセルシェーダをセット
		pDeviceContext->VSSetConstantBuffers(0, 1, &pConstantBuffer);                   // 定数バッファをセット
		pDeviceContext->RSSetState(pRasterizerState);                                   // ラスタライザの設定

		/*

		ここまでやるとポリゴンが描画された

		*/

		return 0;
	}
	case WM_DESTROY:

		pDevice->Release();
		pSwapChain->Release();
		pDeviceContext->Release();
		pRenderTargetView->Release();
		pInputLayout->Release();
		pConstantBuffer->Release();
		pVertexBuffer->Release();
		pRasterizerState->Release();
		pVertexShader->Release();
		pPixelShader->Release();

		PostQuitMessage(0);

		return 0;
	}

	return DefWindowProc(hWnd, uMsg, wparam, lparam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
{
	WNDCLASS wc{ 0 };
	wc.lpfnWndProc = WinProc; // ウィンドウプロシージャ
	wc.hInstance = hInstance; // HINSTANCEの設定
	wc.lpszClassName = "TempWindow"; // クラスネーム
	RegisterClass(&wc); // ウィンドウクラスの登録

	// ウィンドウの作成
	HWND windowHandle = CreateWindow(
		"TempWindow",               // クラスネーム
		"DirectX11_Study",          // ウインドウの名前
		WS_OVERLAPPEDWINDOW,        // ウィンドウスタイル
		960 - (WINDOW_WIDTH / 2),  // ウィンドウ表示位置（x）
		540 - (WINDOW_HEIGHT / 2),  // ウィンドウ表示位置（y）
		WINDOW_WIDTH,               // ウィンドウサイズ（x）
		WINDOW_HEIGHT,              // ウィンドウサイズ（y）
		NULL,
		NULL,
		hInstance,                  // インスタンスハンドル
		NULL
	);

	// NULLチェック
	if (windowHandle == NULL) return 0;

	// ウィンドウの表示
	ShowWindow(
		windowHandle, // ウィンドウハンドル
		nCmdShow
	);

	// メッセージループ
	MSG msg = { 0 };

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg); // メッセージの翻訳
			DispatchMessage(&msg);  // ウィンドウプロシージャにメッセージを送信

			/*

			ここらへんのことをすると、ウィンドウを動かせたり、
			×ボタンが押せたりする

			*/
		}
		else
		{
			// 色指定
			float clearColor[4] = { 0.0, 0.0, 0.0, 1.0 };
			// 画面のクリア
			pDeviceContext->ClearRenderTargetView(pRenderTargetView, clearColor);

			// パラメータの計算
			XMVECTOR eyePos = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);    // 視点位置
			XMVECTOR eyeLookAt = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f); // 視点方向
			XMVECTOR eyeUp = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);     // 視点上方向
			XMMATRIX view = XMMatrixLookAtLH(eyePos, eyeLookAt, eyeUp); // ビュー行列
			XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, (FLOAT)clientWidth / (FLOAT)clientHeight, 0.1f, 110.0f);


			// パラメータの受け渡し
			D3D11_MAPPED_SUBRESOURCE pData;
			CONSTANT_BUFFER cb;
			cb.mWVP = XMMatrixTranspose(view * proj);
			pDeviceContext->Map(pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData);
			memcpy_s(pData.pData, pData.RowPitch, (void*)(&cb), sizeof(cb));
			pDeviceContext->Unmap(pConstantBuffer, 0);

			// 描画
			pDeviceContext->Draw(4, 0);

			// 背景色がclearColorで指定した色で塗りつぶされた。
			// ClearRenderTargetViewをしていること、
			// レンダーターゲットが作成されていることが条件
			pSwapChain->Present(0, 0);
		}
	}

	return 0;
}