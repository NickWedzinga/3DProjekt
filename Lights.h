#pragma once

#ifndef LIGHTS_H
#define LIGHTS_H

#include "includes.h"
#include "OBJLoader.h"

class Lights
{
private:
	//XMFLOAT3 noLights;
	//XMFLOAT3 position[5];
	//XMFLOAT3 intensity[5];
	//XMFLOAT3 color[5];
	//XMFLOAT3 direction[5]; //for spotlights, 0,0,0 if not spotlight
	//XMMATRIX View[5];
	//XMMATRIX Proj[5];
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
	ID3D11RenderTargetView* lRTV[1];
	ID3D11ShaderResourceView* lSRV[1];
	ID3D11Texture2D* lT[1];
	ID3D11VertexShader* vertexShader = nullptr;
	ID3D11VertexShader* vertexShader2 = nullptr;
	ID3D11GeometryShader* geometryShader = nullptr;
	ID3D11PixelShader* pixelShader = nullptr;
	

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