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
