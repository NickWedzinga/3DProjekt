#ifndef DEFERREDRENDERING_H
#define DEFERREDRENDERING_H

#include <d3d11.h>
#include <d3dcompiler.h>
#include <SimpleMath.h>
#include <SimpleMath.inl>
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

#define WIDTH 640.0f
#define HEIGHT 480.0f

struct LightBuffer
{
	DirectX::XMVECTOR lightDirection;
	float padding;
};



class DeferredRendering
{
public:
	void CreateLightBuffer();
	void lightbuffer(ID3D11Device* gDevice);
	void CreateRenderTargets(ID3D11Device* gDevice);
	void InitializeLightShader(ID3D11Device* gDevice);

	LightBuffer lData;
	ID3D11Buffer* gLightBuffer;
	ID3D11RenderTargetView* gRTVA[2];
	ID3D11ShaderResourceView* gSRVA[2];
	ID3D11VertexShader* gVertexShaderLight = nullptr;
	ID3D11PixelShader* gPixelShaderLight = nullptr;
private:
	ID3D11Texture2D* gRTTA[2];
};

#endif