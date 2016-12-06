#include "Billboard.h"
#include <iostream>
#include <time.h>

Billboard::Billboard(int ID)
{
	srand(time(NULL));
	VertexData vertex;
	vertex.ID = ID;
	for (unsigned int i = 0; i < 4096/2; ++i)		//Because lags
	{
		vertex.normal = XMFLOAT3(0, 0, 0);
		vertex.position = XMFLOAT3((rand() % 256), rand() % 100 + 15, (rand() % 256));
		vertex.UV = XMFLOAT2(0.5, 0.5);

		//"Quad", setting ID based on the particle's position
		//if (vertex.position.x >= 256 * 0.4)
		//	vertex.ID += 1;
		//if (vertex.position.z >= 256 * 0.4)
		//	vertex.ID += 2;

		vertices.push_back(vertex);
	}
}

Billboard::~Billboard()
{
	bBBuffer->Release();
}

void Billboard::Init(XMFLOAT3 camPos, ID3D11Device* gDevice)
{	
	InitShaders(gDevice);

	D3D11_BUFFER_DESC vertexBufferDesc;
	memset(&vertexBufferDesc, 0, sizeof(vertexBufferDesc));
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexData)*vertices.size();
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices.data();
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	gDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);
}

void Billboard::Render(ID3D11DeviceContext * gDeviceContext)
{
	gDeviceContext->GSSetShader(geometryShader, nullptr, 0);

	UINT32 stride;
	UINT32 offset;

	stride = sizeof(vertices[0]);
	offset = 0;
	gDeviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	gDeviceContext->PSSetShaderResources(0, 1, &textureView);
	gDeviceContext->Draw(vertices.size(), 0);

}

void Billboard::Update(XMFLOAT3 camPos, ID3D11DeviceContext* gDeviceContext)
{
	for (unsigned int i = 0; i < vertices.size(); ++i)
	{
		vertices[i].position.y -= 0.08f;
		if (vertices[i].position.y < 0)
			vertices[i].position.y = 100;
		gDeviceContext->UpdateSubresource(vertexBuffer, 0/*i*/, NULL, &vertices[0/*i*/], sizeof(VertexData), sizeof(vertices));
	}
}

void Billboard::InitBBBuffer(ID3D11Device* gDevice)
{
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(XMMATRIX);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &rotationMatrix;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	gDevice->CreateBuffer(&cbDesc, &InitData, &bBBuffer);
}



void Billboard::InitShaders(ID3D11Device * gDevice)
{
	ID3D10Blob* pGS = nullptr;

	D3DCompileFromFile(L"GeometryBB.hlsl", nullptr, nullptr, "GS_main", "gs_5_0", 0, 0, &pGS, nullptr);

	gDevice->CreateGeometryShader(pGS->GetBufferPointer(), pGS->GetBufferSize(), nullptr, &geometryShader);

	pGS->Release();
}
