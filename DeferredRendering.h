#pragma once
#ifndef DEFERREDRENDERING_H
#define DEFERREDRENDERING_H

#include "includes.h"
#include "mesh.h"

class DeferredRendering
{
private:
struct LightBuffer
{
	DirectX::XMVECTOR lightDirection;
	float padding;
};
struct IDBuffer
{
	int ID[unsigned int(WIDTH) * unsigned int(HEIGHT)];
};
public:
	void CreateLightBuffer();
	void lightbuffer(ID3D11Device* gDevice);
	void CreateRenderTargets(ID3D11Device* gDevice);
	void InitializeLightShader(ID3D11Device* gDevice);
	void createIDBuffer(vector<Mesh> mesh);
	void makeIDBufferGrateAgain(ID3D11Device* gDevice);
	int Picking();

	IDBuffer IDData;
	ID3D11Buffer* IDBuffer;
	LightBuffer lData;
	ID3D11Buffer* gLightBuffer;
	ID3D11RenderTargetView* gRTVA[4/*2*/];
	ID3D11ShaderResourceView* gSRVA[4/*2*/];
	ID3D11VertexShader* gVertexShaderLight = nullptr;
	ID3D11PixelShader* gPixelShaderLight = nullptr;
private:
	ID3D11Texture2D* gRTTA[4/*2*/];
};

#endif