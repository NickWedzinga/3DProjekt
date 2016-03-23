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
#include <sstream> //obj
#include <fstream> //obj
#include <vector> //obj
#include <DDSTextureLoader.h>
#include <WICTextureLoader.h>

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

ID3D11Buffer* gVertexBuffer = nullptr;
ID3D11Buffer* gConstantBuffer = nullptr;
ID3D11Buffer* pConstantBuffer = nullptr;

ID3D11InputLayout* gVertexLayout = nullptr;
ID3D11VertexShader* gVertexShader = nullptr;
ID3D11GeometryShader* gGeometryShader = nullptr;
ID3D11PixelShader* gPixelShader = nullptr;

ID3D11RenderTargetView* gRTVA[2];
ID3D11Texture2D* gRTTA[2];
ID3D11ShaderResourceView* gSRVA[2];
ID3D11VertexShader* gVertexShaderLight;
ID3D11PixelShader* gPixelShaderLight;
ID3D11InputLayout* gLayoutLight;
ID3D11Buffer* gLightBuffer;

ID3D11SamplerState* sampleState = 0;
ID3D11ShaderResourceView* textureView;

using namespace DirectX::SimpleMath;
using namespace DirectX; //Verkar som man kan ha fler än 1 using namespace, TIL.
using namespace std;

struct CONSTANT_BUFFER
{
	XMMATRIX WorldMatrix;
	XMMATRIX ViewMatrix;
	XMMATRIX ProjMatrix;
};

struct CONSTANT_BUFFER2
{
	XMFLOAT4 KD;
	XMFLOAT4 KA;
	XMFLOAT4 KS;
};

struct LightBuffer
{
	XMVECTOR lightDirection;
	float padding;
};



TwBar *gMyBar;
float background[3]{0, 0, 0};

struct VertexData { float x, y, z, u, v, x2, y2, z2; }; //behövs globalt så att draw kan sättas dynamiskt
vector<VertexData> triangleVertices; //behövs globalt så att draw kan sättas dynamiskt					

float scaleZ = -3;
float angleX = 0;
float angle = 0;
float angleZ = 0;



CONSTANT_BUFFER cData;
CONSTANT_BUFFER2 materialData;
LightBuffer lData;

void CreateLightBuffer()
{
	XMFLOAT3 lightDir = XMFLOAT3(0.0f, 0.0f, 1.0f);
	lData.lightDirection = XMLoadFloat3(&lightDir);
}

void CreateWorldMatrix()
{
	cData.WorldMatrix = XMMatrixRotationRollPitchYaw(angleX, angle, angleZ);
}

void CreateViewMatrix()
{
	XMFLOAT3 cam1 = XMFLOAT3(0, 0, scaleZ);
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
	CreateLightBuffer();

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

	D3D11_BUFFER_DESC lightBufferDesc;
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBuffer);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA InitLightData;
	InitLightData.pSysMem = &lData;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	gDevice->CreateBuffer(&lightBufferDesc, &InitLightData, &gLightBuffer);
	HRESULT hr = gDevice->CreateBuffer(&cbDesc, &InitData, &gConstantBuffer);
}

void constantBuffer2()
{
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(CONSTANT_BUFFER2);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &materialData;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	HRESULT hr = gDevice->CreateBuffer(&cbDesc, &InitData, &pConstantBuffer);
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

void CreateRenderTarget()
{
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = WIDTH;
	textureDesc.Height = HEIGHT;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	
	for (int i = 0; i < 2; i++)
		gDevice->CreateTexture2D(&textureDesc, NULL, &gRTTA[i]);

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	for (int i = 0; i < 2; i++)
		gDevice->CreateRenderTargetView(gRTTA[i], &renderTargetViewDesc, &gRTVA[i]);

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	for (int i = 0; i < 2; i++)
		gDevice->CreateShaderResourceView(gRTTA[i], &shaderResourceViewDesc, &gSRVA[i]);

}

void InitializeLightShader()
{
	ID3D10Blob* pVS = nullptr;
	ID3D10Blob* pPS = nullptr;
	D3D11_SAMPLER_DESC samplerDesc;

	D3DCompileFromFile(L"VertexShaderLights.hlsl", nullptr, nullptr, "LightVertexShader", "vs_4_0", 0, 0, &pVS, nullptr);

	D3DCompileFromFile(L"PixelShaderLights.hlsl", nullptr, nullptr, "LightPixelShader", "ps_4_0", 0, 0, &pPS, nullptr);

	gDevice->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &gVertexShaderLight);
	gDevice->CreatePixelShader(pPS->GetBufferPointer(), pPS->GetBufferSize(), nullptr, &gPixelShaderLight);

	pVS->Release();
	pPS->Release();

}

void Texture(string material)
{
	wchar_t mat[20];
	MultiByteToWideChar(CP_UTF8, 0, material.c_str(), -1, mat, sizeof(mat) / sizeof(wchar_t));

	CreateWICTextureFromFile(gDevice, mat, NULL, &textureView);

	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	gDevice->CreateSamplerState(&samplerDesc, &sampleState);
}

void MTLLoader(string mtlfile)
{
	//mtlfile was sent from OBJLOADER, mtlfile == "box.mtl"
	string myfile(mtlfile), line2, special, material;
	XMFLOAT4 Kd, Ka, Ks;
	ifstream file(myfile);
	istringstream inputString2;

	while (getline(file, line2))
	{
		inputString2.str(line2);
		if (line2.substr(0, 3) == "map")
		{
			inputString2 >> special >> material; //material == "cube_box.jpg"

			Texture(material);
		}
		else if (line2.substr(0, 3) == "Kd ") //En material, RGB Diffuse
		{
			inputString2 >> special >> Kd.x >> Kd.y >> Kd.z;
			materialData.KD = Kd;
		}
		else if (line2.substr(0, 3) == "Ka ")
		{
			inputString2 >> special >> Ka.x >> Ka.y >> Ka.z;
			materialData.KA = Ka;
		}
		else if (line2.substr(0, 3) == "Ks ")
		{
			inputString2 >> special >> Ks.x >> Ks.y >> Ks.z;
			materialData.KS = Ks;
		}
		inputString2.clear();
	}
}

void OBJLoader()
{
	string myFile("sphere1.obj"), special, line2, mtl;
	ifstream file(myFile);
	istringstream inputString;
	struct VertexV { float x, y, z; }; //skapar struct med x, y, z värden
	struct VertexVT { float u, v; };
	struct VertexVN { float x2, y2, z2; };

	vector<VertexV> vertices;
	vector<VertexVT> vertices2;
	vector<VertexVN> vertices3;
	VertexV vtx = { 0, 0, 0 };
	VertexVT vtx2 = { 0, 0 };
	VertexVN vtx3 = { 0, 0, 0 };

	int i = 0;
	UINT valueV = 0;
	UINT valueVT = 0;
	UINT valueVN = 0;
	char valueSlash = 0;
	while (getline(file, line2))
	{
		inputString.str(line2);
		if (line2.substr(0, 2) == "v ")
		{
			inputString >> special >> vtx.x >> vtx.y >> vtx.z;
			vertices.push_back(vtx);
		}
		else if (line2.substr(0, 3) == "vt ")
		{
			inputString >> special >> vtx2.u >> vtx2.v;
			vtx2.u = 1 - vtx2.u; //Because "Maya"
			vtx2.v = 1 - vtx2.v; //Because "Maya"
			vertices2.push_back(vtx2);
		}
		else if (line2.substr(0, 3) == "vn ")
		{
			inputString >> special >> vtx3.x2 >> vtx3.y2 >> vtx3.z2;
			vertices3.push_back(vtx3);
		}
		else if (line2.substr(0, 2) == "f ")
		{
			VertexData temp;
			for (int j = 0; j < 3; j++)
			{
				if (j == 0)
				{
					inputString >> special >> valueV >> valueSlash >> valueVT >> valueSlash >> valueVN;
				}
				if (j == 1 || j == 2)
				{
					inputString >> valueV >> valueSlash >> valueVT >> valueSlash >> valueVN;
				}
				temp.x = vertices.at(valueV - 1).x;
				temp.y = vertices.at(valueV - 1).y;
				temp.z = vertices.at(valueV - 1).z;

				if (valueVT != 0)
				{
					temp.u = vertices2.at(valueVT - 1).u;
					temp.v = vertices2.at(valueVT - 1).v;
				}
				else if (valueVT == 0)
				{
					temp.u = 0.0f;
					temp.v = 0.0f;
				}
				temp.x2 = vertices3.at(valueVN - 1).x2;
				temp.y2 = vertices3.at(valueVN - 1).y2;
				temp.z2 = vertices3.at(valueVN - 1).z2;
				//pushback
				triangleVertices.push_back(temp);
				i++;
			}
		}
		else if (line2.substr(0, 7) == "mtllib ")
		{
			inputString >> special >> mtl;
			MTLLoader(mtl);
		}
		inputString.clear();

	}
	file.close();

	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(VertexData)*triangleVertices.size();
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = triangleVertices.data();
	gDevice->CreateBuffer(&bufferDesc, &vertexData, &gVertexBuffer);

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
		{ "UV", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12 /*kanske 12*/, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0 /*kanske 12?*/, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
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
	HRESULT hr2 = gDeviceContext->Map(gConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, &cData, sizeof(cData));
	gDeviceContext->Unmap(gConstantBuffer, 0);
	HRESULT hr3 = gDeviceContext->Map(pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, &materialData, sizeof(materialData));
	gDeviceContext->Unmap(pConstantBuffer, 0);
}

void Render()
{
	gDeviceContext->OMSetRenderTargets(2, gRTVA, gDepthStencilView);
	// clear the back buffer to a deep blue
	float clearColor[] = { background[0], background[1], background[2], 1 }; //Så att tweakbar kan användas?
	for (int i = 0; i < 2;i++)
		gDeviceContext->ClearRenderTargetView(gRTVA[i], clearColor); //Clear åt zbuffer
	gDeviceContext->ClearDepthStencilView(gDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0); //Clear åt zbuffer

	

	gDeviceContext->VSSetShader(gVertexShader, nullptr, 0);
	gDeviceContext->HSSetShader(nullptr, nullptr, 0);
	gDeviceContext->DSSetShader(nullptr, nullptr, 0);
	gDeviceContext->GSSetShader(gGeometryShader, nullptr, 0);
	gDeviceContext->PSSetShader(gPixelShader, nullptr, 0);
	
	gDeviceContext->PSSetShaderResources(0, 1, &textureView); //Pipelina texturen
	
	UINT32 vertexSize = sizeof(float) * 8; //ändrades från 6
	//UINT32 vertexSize = sizeof(TriangleVertex); //Hade varit snyggt, men hur kommer jag åt den?
	UINT32 offset = 0;

	gDeviceContext->IASetVertexBuffers(0, 1, &gVertexBuffer, &vertexSize, &offset);
	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gDeviceContext->IASetInputLayout(gVertexLayout);

	gDeviceContext->GSSetConstantBuffers(0, 1, &gConstantBuffer);
	gDeviceContext->PSSetConstantBuffers(0, 1, &pConstantBuffer);
	

	gDeviceContext->Draw(triangleVertices.size(), 0);
	//gDeviceContext->Draw(36, 0);


	gDeviceContext->OMSetRenderTargets(1, &gBackbufferRTV, gDepthStencilView);
	gDeviceContext->ClearRenderTargetView(gBackbufferRTV, clearColor);

	gDeviceContext->VSSetShader(gVertexShaderLight, nullptr, 0);
	gDeviceContext->HSSetShader(nullptr, nullptr, 0);
	gDeviceContext->DSSetShader(nullptr, nullptr, 0);
	gDeviceContext->GSSetShader(nullptr, nullptr, 0);
	gDeviceContext->PSSetShader(gPixelShaderLight, nullptr, 0);

	gDeviceContext->PSSetShaderResources(0, 2, gSRVA);

	gDeviceContext->PSSetConstantBuffers(0, 1, &gLightBuffer);

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

		constantBuffer();
		constantBuffer2();
		CreateRenderTarget();
		OBJLoader();	//4. Ersätter triangleData

		TwInit(TW_DIRECT3D11, gDevice); // for Direct3D 11
		TwWindowSize(WIDTH,HEIGHT);

		gMyBar = TwNewBar("KekCity");
		TwAddVarRW(gMyBar, "Background color", TW_TYPE_COLOR3F, &background, "");
		TwAddVarRW(gMyBar, "RotationX", TW_TYPE_FLOAT, &angleX, "min=0.00001 max=360 step=0.1");
		TwAddVarRW(gMyBar, "RotationY", TW_TYPE_FLOAT, &angle, "min=0.00001 max=360 step=0.1");
		TwAddVarRW(gMyBar, "RotationZ", TW_TYPE_FLOAT, &angleZ, "min = 0.00001 max = 360 step = 0.1");

		CreateShaders(); //5. Skapa vertex- och pixel-shaders
		InitializeLightShader();

		zbuffer(); //mad bufferz
		

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