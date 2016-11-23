#include "DeferredRendering.h"

using namespace DirectX::SimpleMath;
using namespace DirectX;
using namespace std;



void DeferredRendering::CreateLightBuffer()
{
	XMFLOAT3 lightDir = XMFLOAT3(0.0f, 0.0f, -5.0f);
	//lData.lightPos = XMLoadFloat3(&lightDir);
}

/*void DeferredRendering::Lightbuffer(ID3D11Device* gDevice)
{
	D3D11_BUFFER_DESC lightBufferDesc;
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBuffer);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA InitLightData;
	InitLightData.pSysMem = &lData;
	InitLightData.SysMemPitch = 0;
	InitLightData.SysMemSlicePitch = 0;

	gDevice->CreateBuffer(&lightBufferDesc, &InitLightData, &gLightBuffer);
}*/

void DeferredRendering::CreateRenderTargets(ID3D11Device* gDevice)
{
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = WIDTH;
	textureDesc.Height = HEIGHT;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	for (int i = 0; i < 3; i++)
		gDevice->CreateTexture2D(&textureDesc, NULL, &gRTTA[i]);

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	for (int i = 0; i < 3; i++)
		gDevice->CreateRenderTargetView(gRTTA[i], &renderTargetViewDesc, &gRTVA[i]);

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	for (int i = 0; i < 3; i++)
		gDevice->CreateShaderResourceView(gRTTA[i], &shaderResourceViewDesc, &gSRVA[i]);

}

void DeferredRendering::InitializeLightShader(ID3D11Device* gDevice)
{
	ID3D10Blob* pVS = nullptr;
	ID3D10Blob* pPS = nullptr;

	D3DCompileFromFile(L"VertexShaderLights.hlsl", nullptr, nullptr, "LightVertexShader", "vs_5_0", 0, 0, &pVS, nullptr);
	D3DCompileFromFile(L"PixelShaderLights.hlsl", nullptr, nullptr, "LightPixelShader", "ps_5_0", 0, 0, &pPS, nullptr);

	gDevice->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &gVertexShaderLight);
	gDevice->CreatePixelShader(pPS->GetBufferPointer(), pPS->GetBufferSize(), nullptr, &gPixelShaderLight);

	pVS->Release();
	pPS->Release();
}

int DeferredRendering::Picking(ID3D11DeviceContext* gDeviceContext)
{
	ID3D11Resource* idRes = nullptr;

	int* intDataPointer = nullptr;
	PickingBuffer->GetResource(&idRes);
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	gDeviceContext->Map(idRes, 0, D3D11_MAP_READ, 0, &mappedResource);
	intDataPointer = (int*)mappedResource.pData;
	int pickID = intDataPointer[0];
	gDeviceContext->Unmap(idRes, 0);
	return pickID;
}

void DeferredRendering::CreatePickingBuffer(ID3D11Device * gDevice)
{
	ID3D11Buffer* buffer = nullptr;
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.ByteWidth = sizeof(int) * HEIGHT * WIDTH;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = sizeof(int);
	gDevice->CreateBuffer(&desc, NULL, &buffer);

	D3D11_UNORDERED_ACCESS_VIEW_DESC unDesc;
	ZeroMemory(&unDesc, sizeof(unDesc));
	unDesc.Format = DXGI_FORMAT_UNKNOWN;
	unDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	unDesc.Buffer.FirstElement = 0;
	unDesc.Buffer.NumElements = 1;
	unDesc.Buffer.Flags = 0;
	gDevice->CreateUnorderedAccessView(buffer, &unDesc, &PickingBuffer);

	buffer->Release();
}

void DeferredRendering::Render(ID3D11DeviceContext* gDeviceContext, ID3D11Buffer* lights)
{
	gDeviceContext->VSSetShader(gVertexShaderLight, nullptr, 0);
	gDeviceContext->GSSetShader(nullptr, nullptr, 0);
	gDeviceContext->PSSetShader(gPixelShaderLight, nullptr, 0);

	gDeviceContext->PSSetShaderResources(0, 3, gSRVA);

	gDeviceContext->PSSetConstantBuffers(0, 1, &lights);

	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
}

void DeferredRendering::SetRenderTargets(ID3D11DeviceContext * gDeviceContext, ID3D11DepthStencilView * gDepthStencilView)
{
	float clearColor[] = { 0.5f, 0.5f, 0.5f, 1 };

	gDeviceContext->OMSetRenderTargets(3, gRTVA, gDepthStencilView);
	gDeviceContext->ClearRenderTargetView(gRTVA[0], clearColor);
	gDeviceContext->ClearRenderTargetView(gRTVA[1], clearColor);
	gDeviceContext->ClearRenderTargetView(gRTVA[2], clearColor);
	gDeviceContext->ClearDepthStencilView(gDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0); //Clear åt zbuffer
}
