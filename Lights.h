#pragma once

#ifndef LIGHTS_H
#define LIGHTS_H

#include "includes.h"
#include "OBJLoader.h"

class Lights
{
private:
	struct lightData
	{
		XMMATRIX View;
		XMMATRIX Proj;
		XMFLOAT3 noLights;
		XMFLOAT3 position;
		XMFLOAT3 intensity;
		XMFLOAT3 color;
		XMFLOAT3 direction; //for spotlights, 0,0,0 if not spotlight
		XMFLOAT3 distance;
	};
	ID3D11ShaderResourceView* lSRV[1];
	ID3D11Texture2D* lT[1];
	ID3D11VertexShader* vertexShader = nullptr;
	
	void InitShaders(ID3D11Device* gDevice);
	void CreateRenderTargets(ID3D11Device* gDevice);
public:
	Lights();
	~Lights();

	lightData lights;
	ID3D11DepthStencilView* lightsDS = nullptr;

	void Init(unsigned int lights, Object* cube, ID3D11Device* gDevice);
	void Render(ID3D11DeviceContext* gDeviceContext);
	void SetShaderResources(ID3D11DeviceContext* gDeviceContext);

	ID3D11Buffer* lightBuffer = nullptr;
};

#endif