//--------------------------------------------------------------------------------------
// BTH - Stefan Petersson 2014.
//--------------------------------------------------------------------------------------
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <AntTweakBar.h>
#include <SimpleMath.h>
#include <SimpleMath.inl>
#include <DirectXMath.h>
#include "bth_image.h"
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

#define WIDTH 640
#define HEIGHT 480

HWND InitWindow(HINSTANCE hInstance);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HRESULT CreateDirect3DContext(HWND wndHandle);

IDXGISwapChain* gSwapChain = nullptr;
ID3D11Device* gDevice = nullptr;
ID3D11DeviceContext* gDeviceContext = nullptr;
ID3D11RenderTargetView* gBackbufferRTV = nullptr;
ID3D11DepthStencilView* gDepthStencilView = nullptr;

ID3D11Buffer* gVertexBuffer = nullptr;
ID3D11Buffer* gConstantBuffer = nullptr;

ID3D11InputLayout* gVertexLayout = nullptr;
ID3D11VertexShader* gVertexShader = nullptr;
ID3D11GeometryShader* gGeometryShader = nullptr;
ID3D11PixelShader* gPixelShader = nullptr;

ID3D11ShaderResourceView* textureView;

TwBar *gMyBar;
float background[3]{0, 0, 0};

using namespace DirectX::SimpleMath;
using namespace DirectX; //Verkar som man kan ha fler än 1 using namespace, TIL.

float angle = 0;

struct CONSTANT_BUFFER 
{
	XMMATRIX WorldMatrix;
	XMMATRIX ViewMatrix;
	XMMATRIX ProjMatrix;
	XMFLOAT4 Color = XMFLOAT4(1, 0, 0, 1);
};

CONSTANT_BUFFER cData;

void CreateWorldMatrix()
{
	cData.WorldMatrix = XMMatrixRotationY(angle);
}

void CreateViewMatrix()
{
	XMFLOAT3 cam1 = XMFLOAT3(0, 0, -3);
	XMFLOAT3 eye1 = XMFLOAT3(0, 0, 0);
	XMFLOAT3 at1 = XMFLOAT3(0, 1, 0);

	XMVECTOR cam = XMLoadFloat3(&cam1);
	XMVECTOR eye = XMLoadFloat3(&eye1);
	XMVECTOR up = XMLoadFloat3(&at1);

	cData.ViewMatrix = XMMatrixLookAtLH(cam, eye, up);
}

void CreateProjMatrix()
{
	float Near = 0.5;
	float Far = 20;
	float FOV = XM_PI * 0.45;

	cData.ProjMatrix = XMMatrixPerspectiveFovLH(FOV, WIDTH/HEIGHT, Near, Far);
}

void constantBuffer()
{
	CreateWorldMatrix();
	CreateViewMatrix();
	CreateProjMatrix();

	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(CONSTANT_BUFFER);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &cData;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	HRESULT hr = gDevice->CreateBuffer(&cbDesc, &InitData, &gConstantBuffer);
}

void zbuffer()
{
	HRESULT hr;
	ID3D11Texture2D* gDepthStencilBuffer = NULL;
	D3D11_TEXTURE2D_DESC depthDesc;
	depthDesc.Width = 640.0f;
	depthDesc.Height = 480.0f;
	depthDesc.MipLevels = 1;
	depthDesc.ArraySize = 1;
	depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthDesc.SampleDesc.Count = 1;
	depthDesc.SampleDesc.Quality = 0;
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthDesc.CPUAccessFlags = 0;
	depthDesc.MiscFlags = 0;

	hr = gDevice->CreateTexture2D(&depthDesc, NULL, &gDepthStencilBuffer);
	hr = gDevice->CreateDepthStencilView(gDepthStencilBuffer, NULL, &gDepthStencilView);
}

void Texture()
{
	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));
	texDesc.Width = BTH_IMAGE_WIDTH;
	texDesc.Height = BTH_IMAGE_HEIGHT;
	texDesc.MipLevels = texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	ID3D11Texture2D *tex = NULL;
	D3D11_SUBRESOURCE_DATA texData;
	ZeroMemory(&texData, sizeof(texData));
	texData.pSysMem = (void*)BTH_IMAGE_DATA;
	texData.SysMemPitch = BTH_IMAGE_WIDTH * 4 * sizeof(char);

	HRESULT hr = gDevice->CreateTexture2D(&texDesc, &texData, &tex);

	D3D11_SHADER_RESOURCE_VIEW_DESC resViewDesc;
	resViewDesc.Format = texDesc.Format;
	resViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	resViewDesc.Texture2D.MipLevels = texDesc.MipLevels;
	resViewDesc.Texture2D.MostDetailedMip = 0;
	hr = gDevice->CreateShaderResourceView(tex, &resViewDesc, &textureView);
	tex->Release(); //Hur är det med release nu då
}

void CreateShaders()
{
	//create vertex shader
	ID3DBlob* pVS = nullptr;
	D3DCompileFromFile(
		L"Vertex.hlsl", // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"VS_main",		// entry point
		"vs_4_0",		// shader model (target)
		0,				// shader compile options
		0,				// effect compile options
		&pVS,			// double pointer to ID3DBlob		
		nullptr			// pointer for Error Blob messages.
		// how to use the Error blob, see here
		// https://msdn.microsoft.com/en-us/library/windows/desktop/hh968107(v=vs.85).aspx
		);

	gDevice->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &gVertexShader);
	
	//create input layout (verified using vertex shader)
	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA,0}
		//{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	gDevice->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), pVS->GetBufferPointer(), pVS->GetBufferSize(), &gVertexLayout);
	// we do not need anymore this COM object, so we release it.
	pVS->Release();

	//create pixel shader
	ID3DBlob* pPS = nullptr;
	D3DCompileFromFile(
		L"Fragment.hlsl", // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"PS_main",		// entry point
		"ps_4_0",		// shader model (target)
		0,				// shader compile options
		0,				// effect compile options
		&pPS,			// double pointer to ID3DBlob		
		nullptr			// pointer for Error Blob messages.
		// how to use the Error blob, see here
		// https://msdn.microsoft.com/en-us/library/windows/desktop/hh968107(v=vs.85).aspx
		);

	gDevice->CreatePixelShader(pPS->GetBufferPointer(), pPS->GetBufferSize(), nullptr, &gPixelShader);
	// we do not need anymore this COM object, so we release it.
	pPS->Release();

	//create GEOMETRY shader
	ID3DBlob* pGS = nullptr;
	D3DCompileFromFile(
		L"GeometryShader.hlsl", // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"GS",			// entry point
		"gs_4_0",		// shader model (target)
		0,				// shader compile options
		0,				// effect compile options
		&pGS,			// double pointer to ID3DBlob		
		nullptr			// pointer for Error Blob messages.
		);

	gDevice->CreateGeometryShader(pGS->GetBufferPointer(), pGS->GetBufferSize(), nullptr, &gGeometryShader);

	pGS->Release();
}

void CreateTriangleData()
{
	struct TriangleVertex
	{
		float x, y, z;
		float u, v;
	};

	TriangleVertex triangleVertices[6] =
	{
		0.5f, 0.5f, 0.0f,
		1.0f, 0.0f,

		0.5f, -0.5f, 0.0f,
		1.0f, 1.0f,

		-0.5f, 0.5f, 0.0f,
		0.0f, 0.0f,

		-0.5f, 0.5f, 0.0f,
		0.0f, 0.0f,

		0.5f, -0.5f, 0.0f,
		1.0f, 1.0f,

		-0.5f, -0.5f, 0.0f,
		0.0f, 1.0f
	};

	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(triangleVertices);

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = triangleVertices;
	gDevice->CreateBuffer(&bufferDesc, &data, &gVertexBuffer);
}

void SetViewport()
{
	D3D11_VIEWPORT vp;
	vp.Width = (float)WIDTH;
	vp.Height = (float)HEIGHT;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	gDeviceContext->RSSetViewports(1, &vp);
}

void Update()
{
	angle += 0.0001;
	CreateWorldMatrix();

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr2 = gDeviceContext->Map(gConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, &cData, sizeof(cData));
	gDeviceContext->Unmap(gConstantBuffer, 0);
}

void Render()
{
	// clear the back buffer to a deep blue
	float clearColor[] = { background[0], background[1], background[2], 1 }; //Så att tweakbar kan användas?
	gDeviceContext->ClearRenderTargetView(gBackbufferRTV, clearColor); //Clear åt zbuffer
	gDeviceContext->ClearDepthStencilView(gDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0); //Clear åt zbuffer

	gDeviceContext->VSSetShader(gVertexShader, nullptr, 0);
	gDeviceContext->HSSetShader(nullptr, nullptr, 0);
	gDeviceContext->DSSetShader(nullptr, nullptr, 0);
	gDeviceContext->GSSetShader(gGeometryShader, nullptr, 0);
	gDeviceContext->PSSetShader(gPixelShader, nullptr, 0);

	gDeviceContext->PSSetShaderResources(0, 1, &textureView); //Pipelina texturen

	UINT32 vertexSize = sizeof(float) * 5;
	//UINT32 vertexSize = sizeof(TriangleVertex); //Hade varit snyggt, men hur kommer jag åt den?
	UINT32 offset = 0;

	gDeviceContext->IASetVertexBuffers(0, 1, &gVertexBuffer, &vertexSize, &offset);
	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gDeviceContext->IASetInputLayout(gVertexLayout);

	gDeviceContext->GSSetConstantBuffers(0, 1, &gConstantBuffer);

	gDeviceContext->Draw(6, 0);
}

int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
	MSG msg = { 0 };
	HWND wndHandle = InitWindow(hInstance); //1. Skapa fönster
	
	if (wndHandle)
	{
		CreateDirect3DContext(wndHandle); //2. Skapa och koppla SwapChain, Device och Device Context

		SetViewport(); //3. Sätt viewport

		constantBuffer();

		TwInit(TW_DIRECT3D11, gDevice); // for Direct3D 11
		TwWindowSize(WIDTH,HEIGHT);

		gMyBar = TwNewBar("KekCity");
		TwAddVarRW(gMyBar, "Background color", TW_TYPE_COLOR3F, &background, "");

		CreateShaders(); //4. Skapa vertex- och pixel-shaders

		CreateTriangleData(); //5. Definiera triangelvertiser, 6. Skapa vertex buffer, 7. Skapa input layout

		Texture(); //texturladdaren
		zbuffer(); //mad bufferz
		gDeviceContext->OMSetRenderTargets(1, &gBackbufferRTV, gDepthStencilView);

		ShowWindow(wndHandle, nCmdShow);

		while (WM_QUIT != msg.message)
		{
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				Update(); //8.1 Update
				Render(); //8.2 Rendera
				TwDraw();

				gSwapChain->Present(0, 0); //9. Växla front- och back-buffer
			}
		}

		gVertexBuffer->Release();
		//gConstantBuffer->Release(); ??
		gVertexLayout->Release();
		gVertexShader->Release();
		gPixelShader->Release();
		//gGeometryShader->Release(); ??
		gBackbufferRTV->Release();
		gSwapChain->Release();
		gDevice->Release();
		gDeviceContext->Release();
		DestroyWindow(wndHandle);
	}

	return (int) msg.wParam;
}

HWND InitWindow(HINSTANCE hInstance)
{
	WNDCLASSEX wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style          = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc    = WndProc;
	wcex.hInstance      = hInstance;
	wcex.lpszClassName = L"BTH_D3D_DEMO";
	if (!RegisterClassEx(&wcex))
		return false;

	RECT rc = { 0, 0, WIDTH, HEIGHT };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	HWND handle = CreateWindow(
		L"BTH_D3D_DEMO",
		L"BTH Direct3D Demo",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rc.right - rc.left,
		rc.bottom - rc.top,
		nullptr,
		nullptr,
		hInstance,
		nullptr);

	return handle;
}

LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	//Hur fungerar denna?
	if (TwEventWin(hWnd, message, wParam, lParam)) // send event message to AntTweakBar
		return 0; // event has been handled by AntTweakBar
	
	switch (message) 
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;		
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

HRESULT CreateDirect3DContext(HWND wndHandle)
{
	// create a struct to hold information about the swap chain
	DXGI_SWAP_CHAIN_DESC scd;

	// clear out the struct for use
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	// fill the swap chain description struct
	scd.BufferCount = 1;                                    // one back buffer
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
	scd.OutputWindow = wndHandle;                           // the window to be used
	scd.SampleDesc.Count = 1;								// how many multisamples
	scd.SampleDesc.Quality = 0;								// quality
	scd.Windowed = TRUE;                                    // windowed/full-screen mode

	// create a device, device context and swap chain using the information in the scd struct
	HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		NULL,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&scd,
		&gSwapChain,
		&gDevice,
		NULL,
		&gDeviceContext);

	if (SUCCEEDED(hr))
	{
		// get the address of the back buffer
		ID3D11Texture2D* pBackBuffer = nullptr;
		gSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

		// use the back buffer address to create the render target
		gDevice->CreateRenderTargetView(pBackBuffer, NULL, &gBackbufferRTV);
		pBackBuffer->Release();

		// set the render target as the back buffer
		//gDeviceContext->OMSetRenderTargets(1, &gBackbufferRTV, NULL);
	}
	return hr;
}