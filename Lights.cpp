#include "Lights.h"
#include <time.h>
#include <random>

Lights::Lights()
{
}

Lights::~Lights()
{
	vertexShader->Release();
	lightsDS->Release();
	lightBuffer->Release();
	for (int i = 0; i < 1; i++)
	{
		lSRV[i]->Release();
		lT[i]->Release();
	}
}

void Lights::Init(unsigned int lights, Object* cube, ID3D11Device* gDevice)
{
	if (lights < 6)
	{
		this->lights.noLights = XMFLOAT3(lights, lights, lights);
		//for (int i = 0; i < lights; ++i)
		//{
		//	this->lights.position[i] = XMFLOAT3(i * 2 - 4, /*i +*/ 5, -3);
		//	this->lights.intensity[i] = XMFLOAT3(i + 1, i + 1, i + 1);
		//	this->lights.color[i] = XMFLOAT3(i - 1, i, i + 1);
		//	XMStoreFloat3(&this->lights.direction[i], XMLoadFloat3(&XMFLOAT3(0, 0, 0)) - XMLoadFloat3(&this->lights.position[i]));
		//	this->lights.Proj[i] = XMMatrixPerspectiveFovLH(XMConvertToRadians(70), WIDTH / HEIGHT, NEAR, 10);
		//	this->lights.View[i] = XMMatrixLookToLH(XMLoadFloat3(&this->lights.position[i]), XMLoadFloat3(&this->lights.direction[i]), XMLoadFloat3(&XMFLOAT3(0.0f, 1.0f, 0.0f)));
		//}
		int i = 1 ;
		//this->lights.position = XMFLOAT3(i * 2 - 4, /*i +*/ 5, -3);
		this->lights.position = XMFLOAT3(100, 100, -100);
		this->lights.intensity = XMFLOAT3(0.1, 1, 1);
		this->lights.color = XMFLOAT3(1, 1, 1);
		//XMStoreFloat3(&this->lights.direction, XMLoadFloat3(&cube->center) - XMLoadFloat3(&this->lights.position));
		this->lights.direction = XMFLOAT3(-1, -1, 1);
		//this->lights.Proj = XMMatrixPerspectiveFovLH(XMConvertToRadians(70), WIDTH / HEIGHT, NEAR, 400);
		this->lights.Proj = XMMatrixOrthographicLH(WIDTH, HEIGHT, NEAR, 300);
		XMVECTOR right = XMVector3Cross(XMLoadFloat3(&XMFLOAT3(0, 1, 0)), XMLoadFloat3(&this->lights.direction));
		XMVECTOR up = XMVector3Cross(XMLoadFloat3(&this->lights.direction), right);
		this->lights.View = XMMatrixLookToLH(XMLoadFloat3(&this->lights.position), XMLoadFloat3(&this->lights.direction), up);
		this->lights.distance = XMFLOAT3(40, 40, 40);

		D3D11_BUFFER_DESC lightBufferDesc;
		lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		lightBufferDesc.ByteWidth = sizeof(lightData);
		lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		lightBufferDesc.MiscFlags = 0;
		lightBufferDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA InitLightData;
		InitLightData.pSysMem = &this->lights;
		InitLightData.SysMemPitch = 0;
		InitLightData.SysMemSlicePitch = 0;

		gDevice->CreateBuffer(&lightBufferDesc, &InitLightData, &lightBuffer);
		InitShaders(gDevice);
	}

	CreateRenderTargets(gDevice);
}

void Lights::InitShaders(ID3D11Device * gDevice)
{
	HRESULT hr;
	ID3D10Blob* pVS = nullptr;
	pVS = nullptr;
	D3DCompileFromFile(L"VertexShaderShadows.hlsl", nullptr, nullptr, "main", "vs_5_0", 0, 0, &pVS, nullptr);
	hr = gDevice->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &vertexShader);
	pVS->Release();
}

void Lights::CreateRenderTargets(ID3D11Device* gDevice)
{
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = WIDTH;
	textureDesc.Height = HEIGHT;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	for (int i = 0; i < 1/*this->lights.noLights.x*/; i++)
		gDevice->CreateTexture2D(&textureDesc, NULL, &lT[0]);


	D3D11_DEPTH_STENCIL_VIEW_DESC depthDesc;
	ZeroMemory(&depthDesc, sizeof(depthDesc));
	depthDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthDesc.Texture2D.MipSlice;
	
	gDevice->CreateDepthStencilView(lT[0], &depthDesc, &lightsDS);

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	for (int i = 0; i < 1/*this->lights.noLights.x*/; i++)
		gDevice->CreateShaderResourceView(lT[0], &shaderResourceViewDesc, &lSRV[0]);
}

void Lights::Render(ID3D11DeviceContext* gDeviceContext)
{
	gDeviceContext->ClearDepthStencilView(lightsDS, D3D11_CLEAR_DEPTH, 1.0f, 0);
	gDeviceContext->OMSetRenderTargets(0, nullptr, lightsDS);

	gDeviceContext->VSSetShader(vertexShader, nullptr, 0);
	gDeviceContext->GSSetShader(nullptr, nullptr, 0);
	gDeviceContext->PSSetShader(nullptr, nullptr, 0);

	gDeviceContext->VSSetConstantBuffers(0, 1, &lightBuffer);
}

void Lights::SetShaderResources(ID3D11DeviceContext * gDeviceContext)
{
	gDeviceContext->PSSetShaderResources(3, 1, &lSRV[0]);
}
