/*

�l�I�ɃT�C�g�����Ă��낢�뎎���Ă݂����

*/


#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")  // �f�o�C�X�ƃX���b�v�`�F�[���̍쐬�ɕK�v

#include <d3dcompiler.h> // �V�F�[�_�[�̃R���p�C���ɕK�v
#pragma comment(lib, "d3dcompiler.lib")

#include <DirectXMath.h>
#include <Windows.h>

using namespace DirectX;

// ��̒��_�����i�[����\����
struct VERTEX {
	XMFLOAT3 V;
	XMFLOAT4 C;
};

// GPU(�V�F�[�_��)�֑��鐔�l���܂Ƃ߂��\����
struct CONSTANT_BUFFER {
	XMMATRIX mWVP;
};


#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 540

int clientWidth;
int clientHeight;

ID3D11Device*           pDevice;           // �f�o�C�X
IDXGISwapChain*         pSwapChain;        // �X���b�v�`�F�[��
ID3D11DeviceContext*    pDeviceContext;    // �f�o�C�X�R���e�L�X�g
ID3D11RenderTargetView* pRenderTargetView; // �����_�[�^�[�Q�b�g�r���[
ID3D11InputLayout*      pInputLayout;      // �C���v�b�g���C�A�E�g
ID3D11Buffer*           pConstantBuffer;   // �萔�o�b�t�@
ID3D11Buffer*           pVertexBuffer;     // ���_�o�b�t�@
ID3D11RasterizerState*  pRasterizerState;  // ���X�^���C�U�[�X�e�C�g
ID3D11VertexShader*     pVertexShader;     // ���_�V�F�[�_�[
ID3D11PixelShader*      pPixelShader;      // �s�N�Z���V�F�[�_�[

LRESULT CALLBACK WinProc(HWND hWnd, UINT uMsg, WPARAM wparam, LPARAM lparam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
		// �v���O�������I�������郁�b�Z�[�W
		PostQuitMessage(0);
		return 0;

	case WM_CREATE:
	{
		// �p�C�v���C���̏���
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

		// �f�o�C�X�ƃX���b�v�`�F�[���̍쐬
		D3D11CreateDeviceAndSwapChain(
			NULL,
			D3D_DRIVER_TYPE_HARDWARE, // �h���C�o�[�^�C�v
			NULL,
			0,
			&fl,                      // �t�B�[�`���[���x��
			1,
			D3D11_SDK_VERSION,        // SDK�̃o�[�W����
			&scd,                     // �X���b�v�`�F�[���f�X�N
			&pSwapChain,              // �X���b�v�`�F�[��
			&pDevice,                 // �f�o�C�X
			NULL,
			&pDeviceContext           // �f�o�C�X�R���e�L�X�g
		);

		ID3D11Texture2D *pbbTex;
		pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pbbTex);
		// �����_�[�^�[�Q�b�g�̍쐬
		pDevice->CreateRenderTargetView(
			pbbTex,
			NULL,
			&pRenderTargetView // �����_�[�^�[�Q�b�g�r���[
		);
		pbbTex->Release();

		/*

		�����܂Őݒ肷��ƁA�E�B���h�E�̔w�i�F�̎w�肪�\�B

		*/

		// �r���[�|�[�g�̐ݒ�
		D3D11_VIEWPORT vp;
		vp.Width = clientWidth;
		vp.Height = clientHeight;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;

		// �V�F�[�_�[�̐ݒ�
		ID3DBlob* pCompileVS; // ���_�V�F�[�_�[
		ID3DBlob* pCompilePS; // �s�N�Z���V�F�[�_�[
		// ���_�V�F�[�_�[�̃R���p�C��
		D3DCompileFromFile(
			L"Shader/shader.hlsl", // �t�@�C���p�X
			NULL,
			NULL,
			"VS", // �G���g���[�|�C���g
			"vs_5_0",
			NULL,
			0,
			&pCompileVS,
			NULL
		);

		// �V�F�[�_�[�쐬
		pDevice->CreateVertexShader(pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), NULL, &pVertexShader);

		// �s�N�Z���V�F�[�_�[�̃R���p�C��
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

		// �V�F�[�_�[�쐬
		pDevice->CreatePixelShader(pCompilePS->GetBufferPointer(), pCompilePS->GetBufferSize(), NULL, &pPixelShader);

		// ���_���C�A�E�g
		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0  },
		    { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 } // ���_���ƂɐF��t����ۂɒǉ�����
		};

		pDevice->CreateInputLayout(
			layout,
			2, // ���C�A�E�g�ɂ�����i�ʒu�A�F�j�̐��B�ʒu�ƐF������ꍇ�u2�v
			pCompileVS->GetBufferPointer(),
			pCompileVS->GetBufferSize(),
			&pInputLayout
		);
		pCompileVS->Release();
		pCompilePS->Release();

		// �萔�o�b�t�@�̐ݒ�
		D3D11_BUFFER_DESC cb;
		cb.ByteWidth = sizeof(CONSTANT_BUFFER);
		cb.Usage = D3D11_USAGE_DYNAMIC;
		cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cb.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		cb.MiscFlags = 0;
		cb.StructureByteStride = 0;
		pDevice->CreateBuffer(&cb, NULL, &pConstantBuffer);

		// ���_���W
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

		// ���X�^���C�U�̐ݒ�
		D3D11_RASTERIZER_DESC rd = {};
		rd.FillMode = D3D11_FILL_SOLID; // D3D11_FILL_WIREFRAME�Ń��C���[�t���[���AD3D11_FILL_SOLID
		rd.CullMode = D3D11_CULL_NONE;
		rd.FrontCounterClockwise = TRUE;
		pDevice->CreateRasterizerState(&rd, &pRasterizerState);

		// �p�C�v���C���\�z
		UINT stride = sizeof(VERTEX);
		UINT offset = 0;
		pDeviceContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);     // ���_�f�[�^���Z�b�g
		pDeviceContext->IASetInputLayout(pInputLayout);                                 // ���_���C�A�E�g���Z�b�g
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP); // �f�[�^�̓��͎�ނ��w��
		pDeviceContext->OMSetRenderTargets(1, &pRenderTargetView, NULL);                // �����_�[�^�[�Q�b�g�r���[�̃Z�b�g
		pDeviceContext->RSSetViewports(1, &vp);                                         // �r���[�|�[�g�̃Z�b�g
		pDeviceContext->VSSetShader(pVertexShader, NULL, 0);                            // ���_�V�F�[�_���Z�b�g
		pDeviceContext->PSSetShader(pPixelShader, NULL, 0);                             // �s�N�Z���V�F�[�_���Z�b�g
		pDeviceContext->VSSetConstantBuffers(0, 1, &pConstantBuffer);                   // �萔�o�b�t�@���Z�b�g
		pDeviceContext->RSSetState(pRasterizerState);                                   // ���X�^���C�U�̐ݒ�

		/*

		�����܂ł��ƃ|���S�����`�悳�ꂽ

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
	wc.lpfnWndProc = WinProc; // �E�B���h�E�v���V�[�W��
	wc.hInstance = hInstance; // HINSTANCE�̐ݒ�
	wc.lpszClassName = "TempWindow"; // �N���X�l�[��
	RegisterClass(&wc); // �E�B���h�E�N���X�̓o�^

	// �E�B���h�E�̍쐬
	HWND windowHandle = CreateWindow(
		"TempWindow",               // �N���X�l�[��
		"DirectX11_Study",          // �E�C���h�E�̖��O
		WS_OVERLAPPEDWINDOW,        // �E�B���h�E�X�^�C��
		960 - (WINDOW_WIDTH / 2),  // �E�B���h�E�\���ʒu�ix�j
		540 - (WINDOW_HEIGHT / 2),  // �E�B���h�E�\���ʒu�iy�j
		WINDOW_WIDTH,               // �E�B���h�E�T�C�Y�ix�j
		WINDOW_HEIGHT,              // �E�B���h�E�T�C�Y�iy�j
		NULL,
		NULL,
		hInstance,                  // �C���X�^���X�n���h��
		NULL
	);

	// NULL�`�F�b�N
	if (windowHandle == NULL) return 0;

	// �E�B���h�E�̕\��
	ShowWindow(
		windowHandle, // �E�B���h�E�n���h��
		nCmdShow
	);

	// ���b�Z�[�W���[�v
	MSG msg = { 0 };

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg); // ���b�Z�[�W�̖|��
			DispatchMessage(&msg);  // �E�B���h�E�v���V�[�W���Ƀ��b�Z�[�W�𑗐M

			/*

			������ւ�̂��Ƃ�����ƁA�E�B���h�E�𓮂�������A
			�~�{�^�����������肷��

			*/
		}
		else
		{
			// �F�w��
			float clearColor[4] = { 0.0, 0.0, 0.0, 1.0 };
			// ��ʂ̃N���A
			pDeviceContext->ClearRenderTargetView(pRenderTargetView, clearColor);

			// �p�����[�^�̌v�Z
			XMVECTOR eyePos = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);    // ���_�ʒu
			XMVECTOR eyeLookAt = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f); // ���_����
			XMVECTOR eyeUp = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);     // ���_�����
			XMMATRIX view = XMMatrixLookAtLH(eyePos, eyeLookAt, eyeUp); // �r���[�s��
			XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, (FLOAT)clientWidth / (FLOAT)clientHeight, 0.1f, 110.0f);


			// �p�����[�^�̎󂯓n��
			D3D11_MAPPED_SUBRESOURCE pData;
			CONSTANT_BUFFER cb;
			cb.mWVP = XMMatrixTranspose(view * proj);
			pDeviceContext->Map(pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData);
			memcpy_s(pData.pData, pData.RowPitch, (void*)(&cb), sizeof(cb));
			pDeviceContext->Unmap(pConstantBuffer, 0);

			// �`��
			pDeviceContext->Draw(4, 0);

			// �w�i�F��clearColor�Ŏw�肵���F�œh��Ԃ��ꂽ�B
			// ClearRenderTargetView�����Ă��邱�ƁA
			// �����_�[�^�[�Q�b�g���쐬����Ă��邱�Ƃ�����
			pSwapChain->Present(0, 0);
		}
	}

	return 0;
}