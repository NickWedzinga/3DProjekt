#pragma once
#ifndef DEFERREDRENDERING_H
#define DEFERREDRENDERING_H

#include "mesh.h"

class DeferredRendering
{
private:
//struct LightBuffer
//{
//	DirectX::XMVECTOR lightPos;
//	float padding;
//};
public:
	void CreateLightBuffer();
	//void Lightbuffer(ID3D11Device* gDevice);
	void CreateRenderTargets(ID3D11Device* gDevice);
	void InitializeLightShader(ID3D11Device* gDevice);
	int Picking(ID3D11DeviceContext* gDeviceContext);
	void CreatePickingBuffer(ID3D11Device* gDevice);
	void Render(ID3D11DeviceContext* gDeviceContext, ID3D11Buffer* lights);
	void SetRenderTargets(ID3D11DeviceContext* gDeviceContext, ID3D11DepthStencilView* gDepthStencilView);

	//LightBuffer lData;
	//ID3D11Buffer* gLightBuffer;
	ID3D11RenderTargetView* gRTVA[3];
	ID3D11ShaderResourceView* gSRVA[3];
	ID3D11VertexShader* gVertexShaderLight = nullptr;
	ID3D11PixelShader* gPixelShaderLight = nullptr;

	ID3D11UnorderedAccessView* PickingBuffer = nullptr;
private:
	ID3D11Texture2D* gRTTA[3];
};

#endif