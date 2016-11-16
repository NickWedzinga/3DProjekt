#pragma once
#ifndef DEFERREDRENDERING_H
#define DEFERREDRENDERING_H

#include "mesh.h"

class DeferredRendering
{
private:
struct LightBuffer
{
	DirectX::XMVECTOR lightPos;
	float padding;
};
//struct IDBuffer
//{
//	int ID[unsigned int(WIDTH) * unsigned int(HEIGHT)];
//};
public:
	void CreateLightBuffer();
	void lightbuffer(ID3D11Device* gDevice);
	void CreateRenderTargets(ID3D11Device* gDevice);
	void InitializeLightShader(ID3D11Device* gDevice);
	int Picking(ID3D11DeviceContext* gDeviceContext);
	void CreatePickingBuffer(ID3D11Device* gDevice);

	/*IDBuffer IData;*/
	/*ID3D11Buffer* IDBuffer;*/
	LightBuffer lData;
	ID3D11Buffer* gLightBuffer;
	ID3D11RenderTargetView* gRTVA[4];
	ID3D11ShaderResourceView* gSRVA[4];
	ID3D11VertexShader* gVertexShaderLight = nullptr;
	ID3D11PixelShader* gPixelShaderLight = nullptr;

	ID3D11UnorderedAccessView* PickingBuffer = nullptr;
private:
	ID3D11Texture2D* gRTTA[4];
};

#endif