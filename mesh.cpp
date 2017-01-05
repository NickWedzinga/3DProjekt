#include "mesh.h"

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
	textureView->Release();
}

void Mesh::Texture(string material, ID3D11Device* gDevice)
{
	wchar_t mat[45];
	MultiByteToWideChar(CP_UTF8, 0, material.c_str(), -1, mat, sizeof(mat) / sizeof(wchar_t));

	CreateWICTextureFromFile(gDevice, mat, NULL, &textureView);
}

void Mesh::moveMesh(XMFLOAT3 position, ID3D11DeviceContext* gDeviceContext)
{
	XMFLOAT3 change;
	XMStoreFloat3(&change, (XMLoadFloat3(&position) - XMLoadFloat3(&this->center)));
	this->center = position;

	for (unsigned int i = 0; i < this->vertices.size(); ++i)
	{
		vertices[i].position.x += change.x;
		vertices[i].position.y += change.y;
		vertices[i].position.z += change.z;
		gDeviceContext->UpdateSubresource(vertexBuffer, 0, NULL, &vertices[0], sizeof(VertexData), sizeof(vertices));
	}
}
