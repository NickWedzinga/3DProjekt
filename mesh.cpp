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
	return;
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
		gDeviceContext->UpdateSubresource(vertexBuffer, 0/*i*/, NULL, &vertices[0/*i*/], sizeof(VertexData), sizeof(vertices));
	}
}

//void Mesh::InitializeShaders(ID3D11Device * gDevice)
//{
//	ID3D10Blob* pVS = nullptr;
//	ID3D10Blob* pGS = nullptr;
//	ID3D10Blob* pPS = nullptr;
//
//	D3DCompileFromFile(L"Vertex.hlsl", nullptr, nullptr, "VS_main", "vs_4_0", 0, 0, &pVS, nullptr);
//	D3DCompileFromFile(L"Geometry.hlsl", nullptr, nullptr, "GS", "gs_4_0", 0, 0, &pGS, nullptr);
//	D3DCompileFromFile(L"Fragment.hlsl", nullptr, nullptr, "PS_main", "ps_4_0", 0, 0, &pPS, nullptr);
//
//	gDevice->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &vertexShader);
//	gDevice->CreateGeometryShader(pGS->GetBufferPointer(), pGS->GetBufferSize(), nullptr, &geometryShader);
//	gDevice->CreatePixelShader(pPS->GetBufferPointer(), pPS->GetBufferSize(), nullptr, &pixelShader);
//
//	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
//		{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "ID", 0, DXGI_FORMAT_R16_SINT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 }
//	};
//	gDevice->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), pVS->GetBufferPointer(), pVS->GetBufferSize(), &vertexLayout);
//
//
//	pVS->Release();
//	pGS->Release();
//	pPS->Release();
//}
