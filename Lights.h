#pragma once

#ifndef LIGHTS_H
#define LIGHTS_H

#include "includes.h"

class Lights
{
private:
	struct lightData
	{
		XMFLOAT3 noLights;
		XMFLOAT3 position[5];
		XMFLOAT3 intensity[5];
		XMFLOAT3 color[5];
		XMFLOAT3 direction[5]; //for spotlights, 0,0,0 if not spotlight
		XMMATRIX View[5];
		XMMATRIX Proj[5];
	}lights;
	ID3D11RenderTargetView* lRTV[5];
	ID3D11ShaderResourceView* lSRV[5];
	ID3D11Texture2D* lT[5];
	ID3D11GeometryShader* geometryShader = nullptr;
	ID3D11PixelShader* pixelShader = nullptr;

	void InitShaders(ID3D11Device* gDevice);
public:
	Lights();
	~Lights();

	void Init(unsigned int lights, ID3D11Device* gDevice);
	void CreateRenderTargets(ID3D11Device* gDevice);
	void Render(ID3D11DeviceContext* gDeviceContext, ID3D11DepthStencilView* gDepthStencilView);
	void SetShaderResources(ID3D11DeviceContext* gDeviceContext);

	ID3D11Buffer* lightBuffer = nullptr;
};

#endif