#pragma once

#ifndef LIGHTS_H
#define LIGHTS_H

#include "includes.h"
#include "OBJLoader.h"

class Lights
{
private:
	struct lightData		//XMVECTORS ALL THE TIME, CAN'T USE XMFLOAT3!!!!!!
	{
		XMVECTOR position;
		XMVECTOR intensity;
		XMVECTOR direction;
		XMMATRIX View;
		XMMATRIX Proj;
	};
	ID3D11ShaderResourceView* lSRV;
	ID3D11Texture2D* lT;
	ID3D11VertexShader* vertexShader = nullptr;
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