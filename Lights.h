#pragma once

#ifndef LIGHTS_H
#define LIGHTS_H

#include "includes.h"

class Lights
{
private:
	struct lightData
	{
		//XMFLOAT3 noLights;
		//XMFLOAT3 position[5];
		//XMFLOAT3 intensity[5];
		//XMFLOAT3 color[5];
		//XMFLOAT3 direction[5]; //for spotlights, 0,0,0 if not spotlight
		//XMMATRIX View[5];
		//XMMATRIX Proj[5];
		XMFLOAT3 noLights;
		XMFLOAT3 position;
		XMFLOAT3 intensity;
		XMFLOAT3 color;
		XMFLOAT3 direction; //for spotlights, 0,0,0 if not spotlight
		XMMATRIX View;
		XMMATRIX Proj;
	}lights;
	ID3D11RenderTargetView* lRTV[1];
	ID3D11ShaderResourceView* lSRV[1];
	ID3D11Texture2D* lT[1];
	ID3D11VertexShader* vertexShader = nullptr;
	ID3D11GeometryShader* geometryShader = nullptr;
	ID3D11PixelShader* pixelShader = nullptr;
	ID3D11DepthStencilView* lightsDS = nullptr;

	void InitShaders(ID3D11Device* gDevice);
	void CreateRenderTargets(ID3D11Device* gDevice);
public:
	Lights();
	~Lights();

	void Init(unsigned int lights, ID3D11Device* gDevice);
	void Render(ID3D11DeviceContext* gDeviceContext);
	void SetShaderResources(ID3D11DeviceContext* gDeviceContext);

	ID3D11Buffer* lightBuffer = nullptr;
};

#endif