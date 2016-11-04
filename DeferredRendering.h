#pragma once
#ifndef DEFERREDRENDERING_H
#define DEFERREDRENDERING_H

#include "includes.h"

class DeferredRendering
{
private:
struct LightBuffer
{
	DirectX::XMVECTOR lightDirection;
	unsigned int ID;
	float padding;
};
public:
	void CreateLightBuffer();
	void lightbuffer(ID3D11Device* gDevice);
	void CreateRenderTargets(ID3D11Device* gDevice);
	void InitializeLightShader(ID3D11Device* gDevice);
	unsigned int Picking();

	LightBuffer lData;
	ID3D11Buffer* gLightBuffer;
	ID3D11RenderTargetView* gRTVA[3/*2*/];
	ID3D11ShaderResourceView* gSRVA[3/*2*/];
	ID3D11VertexShader* gVertexShaderLight = nullptr;
	ID3D11PixelShader* gPixelShaderLight = nullptr;
private:
	ID3D11Texture2D* gRTTA[3/*2*/];
};

#endif