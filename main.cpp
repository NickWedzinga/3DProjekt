//--------------------------------------------------------------------------------------
// BTH - Stefan Petersson 2014.
//--------------------------------------------------------------------------------------

//#include <AntTweakBar.h>
#include "OBJLoader.h"
#include "DeferredRendering.h"
#include "Terrain.h"
#include "camera.h"
#include "Billboard.h"
#include "includes.h"
#include "Lights.h"
#include "Plane.h"

HWND InitWindow(HINSTANCE hInstance);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HRESULT CreateDirect3DContext(HWND wndHandle);

IDXGISwapChain* gSwapChain = nullptr;
ID3D11Device* gDevice = nullptr;
ID3D11DeviceContext* gDeviceContext = nullptr;
ID3D11RenderTargetView* gBackbufferRTV = nullptr;
ID3D11DepthStencilView* gDepthStencilView = nullptr;
ID3D11Buffer* gWorldViewProjBuffer = nullptr;

using namespace DirectX;
using namespace std;

//TwBar *gMyBar;
float background[3]{0, 0, 0};					

float angleX = 0;
float angleY = 0;
float angleZ = 0;
int gID = -1;


Object cube;
DeferredRendering deferred;
CONSTANT_BUFFER cData;
Terrain* terrain = new Terrain;
Camera* camera = new Camera;
Billboard* billboard = new Billboard;
Lights* lights = new Lights;
Plane* ground = new Plane;
Plane* wall = new Plane;


void CreateWorldMatrix()
{
	cData.WorldMatrix = XMMatrixRotationRollPitchYaw(angleX, angleY, angleZ);
}

void CreateProjMatrix()
{
	cData.ProjMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(90), WIDTH/HEIGHT, NEAR, FAR);
}

void constantBuffer()
{
	CreateWorldMatrix();
	camera->Init(cData.ViewMatrix, cData.camDirection);
	CreateProjMatrix();
	deferred.CreateLightBuffer();
	camera->initKeyBuffer(gDevice);
	billboard->InitBBBuffer(gDevice);

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
	ID3D11Texture2D* gDepthStencilBuffer = nullptr;
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

	D3D11_MAPPED_SUBRESOURCE mappedResource2;
	gDeviceContext->Map(camera->keyDataBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource2);
	memcpy(mappedResource2.pData, &camera->keyData, sizeof(camera->keyData));
	gDeviceContext->Unmap(camera->keyDataBuffer, 0);

	billboard->Update(camera->getPos(), gDeviceContext);
}

void Render()
{
	SetViewport();
	deferred.SetRenderTargets(gDeviceContext, gDepthStencilView);
	//Pipeline 1	//Terrain
	terrain->Render(gDeviceContext);

	gDeviceContext->GSSetConstantBuffers(0, 1, &gWorldViewProjBuffer);

	gDeviceContext->Draw(terrain->vertices.size(), 0);
	
	//Pipeline 2	//Floors and walls
	ground->Render(gDeviceContext);
	wall->Render(gDeviceContext);
	

	//Pipeline 3	//Billboard
	billboard->Render(gDeviceContext);

	//Pipeline 4	//Cube
	gDeviceContext->PSSetSamplers(0, 1, &cube.sampler);
	gDeviceContext->GSSetConstantBuffers(0, 1, &gWorldViewProjBuffer);
	gDeviceContext->PSSetConstantBuffers(0, 1, &camera->keyDataBuffer);
	gDeviceContext->PSSetConstantBuffers(1, 1, &cube.gMaterialBuffer);

	cube.Render(gDeviceContext);

	gDeviceContext->Draw(cube.vertices.size(), 0);

	//Pipeline 5 Shadows
	lights->Render(gDeviceContext);
	//gDeviceContext->VSSetConstantBuffers(1, 1, &gWorldViewProjBuffer);
	
	gDeviceContext->Draw(cube.vertices.size(), 0);
	ground->Render(gDeviceContext);
	wall->Render(gDeviceContext);


	//Pipeline 6 Deferred
	float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	gDeviceContext->PSSetSamplers(0, 1, &cube.sampler);
	gDeviceContext->OMSetRenderTargetsAndUnorderedAccessViews(1, &gBackbufferRTV, gDepthStencilView, 1, 1, &deferred.PickingBuffer, NULL);
	gDeviceContext->ClearRenderTargetView(gBackbufferRTV, clearColor);

	gDeviceContext->PSSetConstantBuffers(0, 1, &gWorldViewProjBuffer);
	deferred.Render(gDeviceContext, lights->lightBuffer);
	lights->SetShaderResources(gDeviceContext);

	gDeviceContext->Draw(4, 0);

	gDeviceContext->ClearState();
}

int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
	MSG msg = { 0 };
	ShowCursor(false);
	HWND wndHandle = InitWindow(hInstance); //1. Skapa fönster
	
	if (wndHandle)
	{
		CoInitializeEx(NULL, COINIT_MULTITHREADED);

		CreateDirect3DContext(wndHandle); //2. Skapa och koppla SwapChain, Device och Device Context

		SetViewport(); //3. Sätt viewport
		
		cube.LoadObject(gDevice);	//4. Ersätter triangleData
		ground->CreatePlane(XMFLOAT3(0, -5, 0), 20, 0, 20, gDevice);
		wall->CreatePlane(XMFLOAT3(0, 0, 10), 20, 10, 0, gDevice);

		lights->Init(1, gDevice);
		CreateTerrain();
		constantBuffer();
		//deferred.Lightbuffer(gDevice);
		cube.materialCB(gDevice);
		cube.NormalTexture("Resources/Normalmaps/normalmap.bmp", gDevice);
		terrain->Texture("Resources/Textures/firstheightmap.jpg", gDevice);
		billboard->Texture("Resources/Textures/SPITHOTFIRE.jpg", gDevice);
		ground->Texture("Resources/Textures/grass.jpg", gDevice);
		wall->Texture("Resources/Textures/brick_16.jpg", gDevice);

		deferred.CreateRenderTargets(gDevice);
		deferred.CreatePickingBuffer(gDevice);
		
		//TwInit(TW_DIRECT3D11, gDevice); // for Direct3D 11
		//TwWindowSize(WIDTH,HEIGHT);

		//gMyBar = TwNewBar("KekCity");
		//TwAddVarRW(gMyBar, "ID: ", TW_TYPE_INT32, &gID, "min=-5 max=300 step=1");
		//TwAddVarRW(gMyBar, "Background color", TW_TYPE_COLOR3F, &background, "");
		//TwAddVarRW(gMyBar, "RotationX", TW_TYPE_FLOAT, &angleX, "min=0.00001 max=360 step=0.1");
		//TwAddVarRW(gMyBar, "RotationY", TW_TYPE_FLOAT, &angleY, "min=0.00001 max=360 step=0.1");
		//TwAddVarRW(gMyBar, "RotationZ", TW_TYPE_FLOAT, &angleZ, "min = 0.00001 max = 360 step = 0.1");

		//5. Skapa vertex- och pixel-shaders
		billboard->Init(camera->getPos(), gDevice);
		deferred.InitializeLightShader(gDevice);
		terrain->InitializeTerrainShaders(gDevice);
		cube.InitializeObjectShaders(gDevice);

		zbuffer(); //mad bufferz
		
		ShowWindow(wndHandle, nCmdShow);

		while (WM_QUIT != msg.message)
		{
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				if (WM_LBUTTONUP == msg.message)
				{
					gID = deferred.Picking(gDeviceContext);
				}
				camera->Update(&msg, cData, terrain->getHeightMapY(XMFLOAT2(camera->getPos().x, camera->getPos().z)));
			}
			else
			{
				Update(); //8.1 Update
				Render(); //8.2 Rendera
				//TwDraw();

				gSwapChain->Present(0, 0); //9. Växla front- och back-buffer
			}
		}

		cube.vertexBuffer->Release();
		//gWorldViewProjBuffer->Release(); ??
		//gVertexLayout->Release();
		//gVertexShader->Release();
		//gPixelShader->Release();
		//gGeometryShader->Release(); ??
		gBackbufferRTV->Release();
		gSwapChain->Release();
		gDevice->Release();
		gDeviceContext->Release();
		gDepthStencilView->Release();
		DestroyWindow(wndHandle);

		delete camera;
		delete terrain;
		delete billboard;
		delete lights;
		delete ground;
		delete wall;

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
		XPOS - WIDTH/2,
		YPOS - HEIGHT/2,
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
	//if (TwEventWin(hWnd, message, wParam, lParam)) // send event message to AntTweakBar
	//	return 0; // event has been handled by AntTweakBar
	
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
	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		NULL,//D3D11_CREATE_DEVICE_DEBUG,
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