#include "Plane.h"

Plane::Plane()
{
}

Plane::~Plane()
{
}

void Plane::CreatePlane(XMFLOAT3 pos, int width, int height, int depth, ID3D11Device* gDevice)
{
	VertexData temp;
	temp.ID = 5;
	temp.normal = XMFLOAT3(0.0f, 0.0f, 0.0f);

	//Triangle 1
	//Upper left
	temp.position.x = pos.x - width / 2;
	temp.position.y = pos.y + height / 2;
	temp.position.z = pos.z + depth / 2;
	temp.UV = XMFLOAT2(0.0f, 0.0f);
	vertices.push_back(temp);

	//Bottom right
	temp.position.x = pos.x + width / 2;
	temp.position.y = pos.y - height / 2;
	temp.position.z = pos.z - depth / 2;
	temp.UV = XMFLOAT2(1.0f, 1.0f);
	vertices.push_back(temp);
	
	//Bottom left
	temp.position.x = pos.x - width / 2;
	temp.position.y = pos.y - height / 2;
	temp.position.z = pos.z - depth / 2;
	temp.UV = XMFLOAT2(0.0f, 1.0f);
	vertices.push_back(temp);

	//Triangle 2
	//Upper left
	temp.position.x = pos.x - width / 2;
	temp.position.y = pos.y + height / 2;
	temp.position.z = pos.z + depth / 2;
	temp.UV = XMFLOAT2(0.0f, 0.0f);
	vertices.push_back(temp);

	//Upper right
	temp.position.x = pos.x + width / 2;
	temp.position.y = pos.y + height / 2;
	temp.position.z = pos.z + depth / 2;
	temp.UV = XMFLOAT2(1.0f, 0.0f);
	vertices.push_back(temp);

	//Bottom right
	temp.position.x = pos.x + width / 2;
	temp.position.y = pos.y - height / 2;
	temp.position.z = pos.z - depth / 2;
	temp.UV = XMFLOAT2(1.0f, 1.0f);
	vertices.push_back(temp);

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

void Plane::Render(ID3D11DeviceContext * gDeviceContext)
{
	gDeviceContext->PSSetShaderResources(0, 1, &textureView);
	UINT32 stride;
	UINT32 offset;
	stride = sizeof(vertices[0]);
	offset = 0;

	gDeviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	gDeviceContext->Draw(vertices.size(), 0);
}
