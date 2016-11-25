#include "Lights.h"
#include <time.h>
#include <random>

Lights::Lights()
{
}

Lights::~Lights()
{
}

void Lights::Init(unsigned int lights, ID3D11Device* gDevice)
{
	if (lights < 6)
	{
		this->lights.noLights = XMFLOAT3(lights, lights, lights);
		for (unsigned int i = 0; i < lights; ++i)
		{
			this->lights.position[i] = XMFLOAT3(i * 2 - 4, /*i +*/ 5, -3);
			this->lights.intensity[i] = XMFLOAT3(i + 1, i + 1, i + 1);
			this->lights.color[i] = XMFLOAT3(i - 1, i, i + 1);
			XMStoreFloat3(&this->lights.direction[i], XMLoadFloat3(&XMFLOAT3(0, 0, 0)) - XMLoadFloat3(&this->lights.position[i]));
			this->lights.Proj[i] = XMMatrixPerspectiveFovLH(XMConvertToRadians(70), WIDTH / HEIGHT, NEAR, 10);
			this->lights.View[i] = XMMatrixLookToLH(XMLoadFloat3(&this->lights.position[i]), XMLoadFloat3(&this->lights.direction[i]), XMLoadFloat3(&XMFLOAT3(0.0f, 1.0f, 0.0f)));
		}
		//for (unsigned int i = 0; i < 5 - lights; ++i)
		//{
		//	this->lights.position[i] = XMFLOAT3(0, 0, 0);
		//	this->lights.intensity[i] = XMFLOAT3(0, 0, 0);
		//	this->lights.color[i] = XMFLOAT3(0, 0, 0);
		//	//XMStoreFloat3(&this->lights.direction[i], XMLoadFloat3(&XMFLOAT3(0, 0, 0)) - XMLoadFloat3(&this->lights.position[i]));
		//	this->lights.Proj[i] = XMMATRIX()/*XMMatrixPerspectiveFovLH(XMConvertToRadians(70), WIDTH / HEIGHT, NEAR, 10)*/;
		//	this->lights.View[i] = XMMATRIX()/*XMMatrixLookToLH(XMLoadFloat3(&this->lights.position[i]), XMLoadFloat3(&this->lights.direction[i]), XMLoadFloat3(&XMFLOAT3(0.0f, 1.0f, 0.0f)))*/;
		//}

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
}

void Lights::InitShaders(ID3D11Device * gDevice)
{
	ID3D10Blob* pGS = nullptr;
	ID3D10Blob* pPS = nullptr;

	D3DCompileFromFile(L"GeometryShaderShadows.hlsl", nullptr, nullptr, "GS_main", "gs_5_0", 0, 0, &pGS, nullptr);
	D3DCompileFromFile(L"PixelShaderShadows.hlsl", nullptr, nullptr, "PS_main", "ps_5_0", 0, 0, &pPS, nullptr);

	gDevice->CreateGeometryShader(pGS->GetBufferPointer(), pGS->GetBufferSize(), nullptr, &geometryShader);
	gDevice->CreatePixelShader(pPS->GetBufferPointer(), pPS->GetBufferSize(), nullptr, &pixelShader);

	pGS->Release();
	pPS->Release();
}

void Lights::CreateRenderTargets(ID3D11Device* gDevice)
{
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = float(WIDTH);
	textureDesc.Height = HEIGHT;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	for (int i = 0; i < 5/*this->lights.noLights.x*/; i++)
		gDevice->CreateTexture2D(&textureDesc, NULL, &lT[i]);

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	for (int i = 0; i < 5/*this->lights.noLights.x*/; i++)
		gDevice->CreateRenderTargetView(lT[i], &renderTargetViewDesc, &lRTV[i]);

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	for (int i = 0; i < 5/*this->lights.noLights.x*/; i++)
		gDevice->CreateShaderResourceView(lT[i], &shaderResourceViewDesc, &lSRV[i]);
}

void Lights::Render(ID3D11DeviceContext * gDeviceContext, ID3D11DepthStencilView* gDepthStencilView)
{
	int numLights = this->lights.noLights.x;
	gDeviceContext->OMSetRenderTargets(5, lRTV, gDepthStencilView);		//Borde inte vara n�gon skillnad med eller utan depth
	gDeviceContext->GSSetShader(geometryShader, nullptr, 0);
	gDeviceContext->PSSetShader(pixelShader, nullptr, 0);

	gDeviceContext->GSGetConstantBuffers(0, 1, &lightBuffer);


	//for (int i = 0; i < numLights; i++)
	//{
		//We can just call draw again and we will draw the cube.
	//}
}

void Lights::SetShaderResources(ID3D11DeviceContext * gDeviceContext)
{
	gDeviceContext->PSSetShaderResources(3, 5, lSRV);
}
