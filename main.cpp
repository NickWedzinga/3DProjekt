//--------------------------------------------------------------------------------------
// BTH - Stefan Petersson 2014.
//--------------------------------------------------------------------------------------
/*#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <AntTweakBar.h>
#include <SimpleMath.h>
#include <SimpleMath.inl>
#include <DirectXMath.h>
#include "bth_image.h"
#include <sstream> //obj
#include <fstream> //obj
#include <vector> //obj
#include <DDSTextureLoader.h>
#include <WICTextureLoader.h>
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")*/

#include <windows.h>
#include <AntTweakBar.h>
#include <DirectXMath.h>
#include <d3d11.h>
#include <d3dcompiler.h>

#include "OBJLoader.h"
#include "DeferredRendering.h"
#include "Terrain.h"
#include "camera.h"

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

#define WIDTH 640.0f
#define HEIGHT 480.0f

HWND InitWindow(HINSTANCE hInstance);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HRESULT CreateDirect3DContext(HWND wndHandle);

IDXGISwapChain* gSwapChain = nullptr;
ID3D11Device* gDevice = nullptr;
ID3D11DeviceContext* gDeviceContext = nullptr;
ID3D11RenderTargetView* gBackbufferRTV = nullptr;
ID3D11DepthStencilView* gDepthStencilView = nullptr;

ID3D11Buffer* gWorldViewProjBuffer = nullptr;

ID3D11InputLayout* gVertexLayout = nullptr;
ID3D11VertexShader* gVertexShader = nullptr;
ID3D11GeometryShader* gGeometryShader = nullptr;
ID3D11PixelShader* gPixelShader = nullptr;


using namespace DirectX::SimpleMath;
using namespace DirectX; //Verkar som man kan ha fler än 1 using namespace, TIL.
using namespace std;


struct CONSTANT_BUFFER
{
	XMMATRIX WorldMatrix;
	XMMATRIX ViewMatrix;
	XMMATRIX ProjMatrix;
	XMVECTOR camDirection;
};

TwBar *gMyBar;
float background[3]{0, 0, 0};					

float angleX = 0;
float angleY = 0;
float angleZ = 0;

float tempx = 0;
float tempz = 0;


Object cube;
DeferredRendering deferred;
CONSTANT_BUFFER cData;
Terrain* terrain = new Terrain;
Camera* camera = new Camera;


void CreateWorldMatrix()
{
	cData.WorldMatrix = XMMatrixRotationRollPitchYaw(angleX, angleY, angleZ);
}

void CreateProjMatrix()
{
	float Near = 0.5;
	float Far = 200;
	float FOV = XM_PI * 0.45;

	cData.ProjMatrix = XMMatrixPerspectiveFovLH(FOV, WIDTH/HEIGHT, Near, Far);
}

void constantBuffer()
{
	CreateWorldMatrix();
	camera->Init(cData.ViewMatrix, cData.camDirection);
	CreateProjMatrix();
	deferred.CreateLightBuffer();

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

	HRESULT hr = gDevice->CreateBuffer(&cbDesc, &InitData, &gWorldViewProjBuffer);
}

void zbuffer()
{
	HRESULT hr;
	ID3D11Texture2D* gDepthStencilBuffer = NULL;
	D3D11_TEXTURE2D_DESC depthDesc;
	depthDesc.Width = WIDTH;
	depthDesc.Height = HEIGHT;
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


void CreateShaders()
{
	//create vertex shader
	ID3DBlob* pVS = nullptr;
	D3DCompileFromFile(
		L"Vertex.hlsl", // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"VS_main",			// entry point
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
		{ "UV", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
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

void CreateTerrain()
{
	terrain->Initialize(gDevice);
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
	CreateWorldMatrix();

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr2 = gDeviceContext->Map(gWorldViewProjBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, &cData, sizeof(cData));
	gDeviceContext->Unmap(gWorldViewProjBuffer, 0);
	HRESULT hr3 = gDeviceContext->Map(cube.gMaterialBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, &cube.materialData, sizeof(cube.materialData));
	gDeviceContext->Unmap(cube.gMaterialBuffer, 0);
	tempx = camera->getPos().x;
	tempz = camera->getPos().z;
}

void Render()
{
	float clearColor[] = { background[0], background[1], background[2], 1 }; //Så att tweakbar kan användas?


	//Pipeline 1
	gDeviceContext->OMSetRenderTargets(1, &deferred.gRTVA[0]/*&gBackbufferRTV*/, gDepthStencilView);
	gDeviceContext->ClearRenderTargetView(deferred.gRTVA[0]/*gBackbufferRTV*/, clearColor);
	gDeviceContext->ClearDepthStencilView(gDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0); //Clear åt zbuffer

	gDeviceContext->VSSetShader(terrain->gVertexShaderT, nullptr, 0);
	gDeviceContext->HSSetShader(nullptr, nullptr, 0);
	gDeviceContext->DSSetShader(nullptr, nullptr, 0);
	gDeviceContext->GSSetShader(terrain->gGeometryShaderT, nullptr, 0);
	gDeviceContext->PSSetShader(terrain->gPixelShaderT, nullptr, 0);

	terrain->Render(gDeviceContext);

	gDeviceContext->GSSetConstantBuffers(0, 1, &gWorldViewProjBuffer);

	gDeviceContext->Draw(terrain->vecVertices.size()/*sizeof(terrain->vertices)*/, 0);


	//Pipeline 2
	gDeviceContext->OMSetRenderTargets(3, deferred.gRTVA, gDepthStencilView);
	for (int i = 1; i < 3; i++)
		gDeviceContext->ClearRenderTargetView(deferred.gRTVA[i], clearColor); //Clear åt zbuffer

	gDeviceContext->ClearDepthStencilView(gDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0); //Clear åt zbuffer



	gDeviceContext->VSSetShader(gVertexShader, nullptr, 0);
	gDeviceContext->HSSetShader(nullptr, nullptr, 0);
	gDeviceContext->DSSetShader(nullptr, nullptr, 0);
	gDeviceContext->GSSetShader(gGeometryShader, nullptr, 0);
	gDeviceContext->PSSetShader(gPixelShader, nullptr, 0);

	gDeviceContext->PSSetShaderResources(0, 1, &cube.textureView); //Pipelina texturen

	UINT32 vertexSize = sizeof(cube.triangleVertices[0]);
	UINT32 offset = 0;

	gDeviceContext->IASetVertexBuffers(0, 1, &cube.VertexBuffer, &vertexSize, &offset);

	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gDeviceContext->IASetInputLayout(gVertexLayout);

	gDeviceContext->GSSetConstantBuffers(0, 1, &gWorldViewProjBuffer);

	gDeviceContext->Draw(cube.triangleVertices.size(), 0);


	//Pipeline 3

	
	gDeviceContext->OMSetRenderTargets(1, &gBackbufferRTV, gDepthStencilView);
	gDeviceContext->ClearRenderTargetView(gBackbufferRTV, clearColor);

	gDeviceContext->VSSetShader(deferred.gVertexShaderLight, nullptr, 0);
	gDeviceContext->HSSetShader(nullptr, nullptr, 0);
	gDeviceContext->DSSetShader(nullptr, nullptr, 0);
	gDeviceContext->GSSetShader(nullptr, nullptr, 0);
	gDeviceContext->PSSetShader(deferred.gPixelShaderLight, nullptr, 0);

	gDeviceContext->PSSetShaderResources(0, 3, deferred.gSRVA);
	//gDeviceContext->PSSetShaderResources(0, 1, &deferred.gSRVA[2]);
	
	gDeviceContext->PSSetConstantBuffers(0, 1, &deferred.gLightBuffer);
	gDeviceContext->PSSetConstantBuffers(1, 1, &cube.gMaterialBuffer);
	gDeviceContext->PSSetConstantBuffers(2, 1, &gWorldViewProjBuffer);

	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	gDeviceContext->Draw(4, 0);

	
}

int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
	MSG msg = { 0 };
	HWND wndHandle = InitWindow(hInstance); //1. Skapa fönster
	
	if (wndHandle)
	{
		CoInitializeEx(NULL, COINIT_MULTITHREADED);

		CreateDirect3DContext(wndHandle); //2. Skapa och koppla SwapChain, Device och Device Context

		SetViewport(); //3. Sätt viewport
		
		cube.LoadObject(gDevice);	//4. Ersätter triangleData

		CreateTerrain();
		constantBuffer();
		deferred.lightbuffer(gDevice);
		cube.materialCB(gDevice);
		deferred.CreateRenderTargets(gDevice);
		

		TwInit(TW_DIRECT3D11, gDevice); // for Direct3D 11
		TwWindowSize(WIDTH,HEIGHT);

		gMyBar = TwNewBar("KekCity");
		TwAddVarRW(gMyBar, "Background color", TW_TYPE_COLOR3F, &background, "");
		TwAddVarRW(gMyBar, "RotationX", TW_TYPE_FLOAT, &angleX, "min=0.00001 max=360 step=0.1");
		TwAddVarRW(gMyBar, "RotationY", TW_TYPE_FLOAT, &angleY, "min=0.00001 max=360 step=0.1");
		TwAddVarRW(gMyBar, "RotationZ", TW_TYPE_FLOAT, &angleZ, "min = 0.00001 max = 360 step = 0.1");
		/*float tempx = camera->getPos().x;
		float tempz = camera->getPos().z;*/

		TwAddVarRW(gMyBar, "Z", TW_TYPE_FLOAT, &tempz, "min=-360 max=360 step=1");
		TwAddVarRW(gMyBar, "X", TW_TYPE_FLOAT, &tempx, "min=-360 max=360 step=1");
		TwAddVarRW(gMyBar, "MouseX", TW_TYPE_FLOAT, &tempx, "min=-360 max=360 step=1");

		CreateShaders(); //5. Skapa vertex- och pixel-shaders
		deferred.InitializeLightShader(gDevice);
		terrain->InitializeTerrainShaders(gDevice);

		zbuffer(); //mad bufferz
		

		ShowWindow(wndHandle, nCmdShow);

		while (WM_QUIT != msg.message)
		{
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				camera->Update(&msg, cData.ViewMatrix, cData.camDirection);
			}
			else
			{
				Update(); //8.1 Update
				Render(); //8.2 Rendera
				TwDraw();

				gSwapChain->Present(0, 0); //9. Växla front- och back-buffer
			}
		}
		
		cube.VertexBuffer->Release();
		//gWorldViewProjBuffer->Release(); ??
		gVertexLayout->Release();
		gVertexShader->Release();
		gPixelShader->Release();
		//gGeometryShader->Release(); ??
		gBackbufferRTV->Release();
		gSwapChain->Release();
		gDevice->Release();
		gDeviceContext->Release();
		DestroyWindow(wndHandle);

		delete camera;
		delete terrain;
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
		D3D11_CREATE_DEVICE_DEBUG,
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