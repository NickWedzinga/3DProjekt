#include "DeferredRendering.h"

using namespace DirectX::SimpleMath;
using namespace DirectX; //Verkar som man kan ha fler än 1 using namespace, TIL.
using namespace std;

void DeferredRendering::CreateLightBuffer()
{
	XMFLOAT3 lightDir = XMFLOAT3(0.0f, 0.0f, 1.0f);
	lData.lightDirection = XMLoadFloat3(&lightDir);
}

void DeferredRendering::lightbuffer(ID3D11Device* gDevice)
{
	D3D11_BUFFER_DESC lightBufferDesc;
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBuffer);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA InitLightData;
	InitLightData.pSysMem = &lData;
	InitLightData.SysMemPitch = 0;
	InitLightData.SysMemSlicePitch = 0;

	gDevice->CreateBuffer(&lightBufferDesc, &InitLightData, &gLightBuffer);
}

void DeferredRendering::CreateRenderTargets(ID3D11Device* gDevice)
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

	for (int i = 0; i < 4/*2*/; i++)
		gDevice->CreateTexture2D(&textureDesc, NULL, &gRTTA[i]);

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	for (int i = 0; i < 4/*2*/; i++)
		gDevice->CreateRenderTargetView(gRTTA[i], &renderTargetViewDesc, &gRTVA[i]);

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	for (int i = 0; i < 4/*2*/; i++)
		gDevice->CreateShaderResourceView(gRTTA[i], &shaderResourceViewDesc, &gSRVA[i]);
}

void DeferredRendering::InitializeLightShader(ID3D11Device* gDevice)
{
	ID3D10Blob* pVS = nullptr;
	ID3D10Blob* pPS = nullptr;

	D3DCompileFromFile(L"VertexShaderLights.hlsl", nullptr, nullptr, "LightVertexShader", "vs_4_0", 0, 0, &pVS, nullptr);

	D3DCompileFromFile(L"PixelShaderLights.hlsl", nullptr, nullptr, "LightPixelShader", "ps_4_0", 0, 0, &pPS, nullptr);

	gDevice->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &gVertexShaderLight);
	gDevice->CreatePixelShader(pPS->GetBufferPointer(), pPS->GetBufferSize(), nullptr, &gPixelShaderLight);

	pVS->Release();
	pPS->Release();
}

void DeferredRendering::createIDBuffer(vector<Mesh> mesh)
{
	
}



void DeferredRendering::makeIDBufferGrateAgain(ID3D11Device * gDevice)
{
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(IDData);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &IDData;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	HRESULT hr = gDevice->CreateBuffer(&cbDesc, &InitData, &IDBuffer);
}

int DeferredRendering::Picking()
{
	return *(IDData.ID);
}
