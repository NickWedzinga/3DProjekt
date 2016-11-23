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
		for (unsigned int i = 0; i < lights; ++i)
		{
			this->lights[i].noLights = XMFLOAT3(lights, lights, lights);
			this->lights[i].position = XMFLOAT3(i - 1, i, i + 1);
			this->lights[i].intensity = XMFLOAT3(i, i, i);
			this->lights[i].color = XMFLOAT3(i - 1, i, i + 1);
			this->lights[i].direction = XMFLOAT3(0, 0, 0);
		}
	}

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
}

void Lights::CreateRenderTargets(ID3D11Device* gDevice)
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

	for (int i = 0; i < this->lights[0].noLights.x; i++)
		gDevice->CreateTexture2D(&textureDesc, NULL, &lT[i]);

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	for (int i = 0; i < this->lights[0].noLights.x; i++)
		gDevice->CreateRenderTargetView(lT[i], &renderTargetViewDesc, &lRTV[i]);

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	for (int i = 0; i < this->lights[0].noLights.x; i++)
		gDevice->CreateShaderResourceView(lT[i], &shaderResourceViewDesc, &lSRV[i]);
}

void Lights::Render(ID3D11DeviceContext * gDeviceContext)
{
	int numLights = this->lights->noLights.x;
	gDeviceContext->OMSetRenderTargets(numLights, lRTV, NULL);


	for (int i = 0; i < numLights; i++)
	{
		//We can just call draw again and we will draw the cube.
	}
}
