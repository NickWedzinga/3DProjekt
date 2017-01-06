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
	lSRV->Release();
	lT->Release();
}

void Lights::Init(unsigned int lights, Object* cube, ID3D11Device* gDevice)
{
	this->lights.position = XMLoadFloat3(&XMFLOAT3(10, 5, -10));
	this->lights.intensity = XMLoadFloat3(&XMFLOAT3(1, 1, 1));

	this->lights.direction = XMLoadFloat3(&XMFLOAT3(0,0,0)) - this->lights.position;
	//this->lights.Proj = XMMatrixPerspectiveFovLH(XM_PI / 4.0f, SMWIDTH/SMHEIGHT, NEAR, 60);
	//this->lights.Proj = XMMatrixTranspose(this->lights.Proj);
	this->lights.Proj = XMMatrixOrthographicLH(SMWIDTH, SMHEIGHT, NEAR, 50);
	XMVECTOR right = XMVector3Cross(XMLoadFloat3(&XMFLOAT3(0, 1, 0)), this->lights.direction);
	XMVECTOR up = XMVector3Cross(this->lights.direction, right);
	this->lights.View = XMMatrixLookToLH(this->lights.position, this->lights.direction, up);
	//this->lights.View = XMMatrixTranspose(this->lights.View);

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

	HRESULT hr = gDevice->CreateBuffer(&lightBufferDesc, &InitLightData, &lightBuffer);
	InitShaders(gDevice);

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

	//pVS = nullptr;
	//D3DCompileFromFile(L"VertexShaderShadows.hlsl", nullptr, nullptr, "VS_main", "vs_5_0", 0, 0, &pVS, nullptr);
	//hr = gDevice->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &vertexShader);
	//pVS->Release();

	//ID3D10Blob* pPS = nullptr;
	//D3DCompileFromFile(L"PixelShaderShadows.hlsl", nullptr, nullptr, "PS_main", "ps_5_0", 0, 0, &pPS, nullptr);
	//hr = gDevice->CreatePixelShader(pPS->GetBufferPointer(), pPS->GetBufferSize(), nullptr, &pixelShader);
	//pPS->Release();


}

void Lights::CreateRenderTargets(ID3D11Device* gDevice)
{
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = SMWIDTH*16;
	textureDesc.Height = SMHEIGHT*16;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	gDevice->CreateTexture2D(&textureDesc, NULL, &lT);


	D3D11_DEPTH_STENCIL_VIEW_DESC depthDesc;
	ZeroMemory(&depthDesc, sizeof(depthDesc));
	depthDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthDesc.Texture2D.MipSlice;
	gDevice->CreateDepthStencilView(lT, &depthDesc, &lightsDS);

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;
	gDevice->CreateShaderResourceView(lT, &shaderResourceViewDesc, &lSRV);
}

void Lights::Render(ID3D11DeviceContext* gDeviceContext)
{
	gDeviceContext->ClearDepthStencilView(lightsDS, D3D11_CLEAR_DEPTH, 1.0f, 0);
	gDeviceContext->OMSetRenderTargets(0, nullptr, lightsDS);

	gDeviceContext->VSSetShader(vertexShader, nullptr, 0);
	gDeviceContext->GSSetShader(nullptr, nullptr, 0);
	gDeviceContext->PSSetShader(pixelShader, nullptr, 0);

	gDeviceContext->VSSetConstantBuffers(0, 1, &lightBuffer);
}

void Lights::SetShaderResources(ID3D11DeviceContext * gDeviceContext)
{
	gDeviceContext->PSSetShaderResources(3, 1, &lSRV);
}
