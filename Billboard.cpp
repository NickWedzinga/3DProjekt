#include "Billboard.h"
#include <iostream>
#include <time.h>
#include "QuadTree.h"

Billboard::Billboard(int ID, QuadTree* quadTree)
{
	srand(time(NULL));
	uint firstLeaf = quadTree->GetFirstLeaf(), sqrtLeaves = sqrt(quadTree->GetNumOfLeaves()), numLeafs = quadTree->GetNumOfLeaves();
	VertexData vertex;
	vertex.ID = ID;
	uint bbsPerNode = 4096 / numLeafs;

	for (uint i = 0; i < sqrtLeaves; ++i)
	{
		for (uint j = 0; j < sqrtLeaves; ++j)
		{
			uint x = sqrtLeaves * j, z = sqrtLeaves * i;
			for (uint k = 0; k < bbsPerNode; ++k) // means we can't have fewer billboards than leaves
			{
				vertex.normal = XMFLOAT3(0, 0, 0);
				vertex.position = XMFLOAT3((rand() % 256 / sqrtLeaves + x), rand() % 100 + 15, (rand() % 256 / sqrtLeaves + z));
				vertex.UV = XMFLOAT2(0.5, 0.5);

				//quadTree->pushVertexIndex(firstLeaf + i  * sqrtLeaves + j, i * pow(sqrtLeaves, 2) + j * sqrtLeaves + k);

				vertices.push_back(vertex);
			}
		}
	}
	quadTree->FillLeaves(0, bbsPerNode);
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
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexData)*vertices.size();
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
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

	stride = sizeof(used[0]);
	offset = 0;
	gDeviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	gDeviceContext->PSSetShaderResources(0, 1, &textureView);
	gDeviceContext->Draw(used.size(), 0);

}

void Billboard::Update(XMFLOAT3 camPos, ID3D11DeviceContext* gDeviceContext)
{
	for (unsigned int i = 0; i < vertices.size(); ++i)	//update is delayed 1 frame
	{
		vertices[i].position.y -= 0.08f;
		if (vertices[i].position.y < 0)
			vertices[i].position.y = 100;
	}
	if (used.size() != 0)
	{
		//gDeviceContext->UpdateSubresource(vertexBuffer, 0, NULL, &used[0], sizeof(VertexData), sizeof(used));
		D3D11_MAPPED_SUBRESOURCE mappedResource; //map/unmap recommended for doing every frame
		HRESULT hr = gDeviceContext->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		memcpy(mappedResource.pData, &used[0], used.size() * sizeof(VertexData));
		gDeviceContext->Unmap(vertexBuffer, 0);
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

void Billboard::MoveToUsed(uint index)
{
	used.push_back(vertices[index]);
}



void Billboard::InitShaders(ID3D11Device * gDevice)
{
	ID3D10Blob* pGS = nullptr;

	D3DCompileFromFile(L"GeometryBB.hlsl", nullptr, nullptr, "GS_main", "gs_5_0", 0, 0, &pGS, nullptr);

	gDevice->CreateGeometryShader(pGS->GetBufferPointer(), pGS->GetBufferSize(), nullptr, &geometryShader);

	pGS->Release();
}
