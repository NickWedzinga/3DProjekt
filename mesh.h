#pragma once
#ifndef MESH_H
#define MESH_H

#include "includes.h"
#include <WICTextureLoader.h>

using namespace std;

class Mesh
{
protected:
	struct VertexData
	{
		XMFLOAT2 UV;
		XMFLOAT3 normal;
		XMFLOAT3 position;
		int ID;
	};
	int ID;

public:
	Mesh();
	~Mesh();
	ID3D11Buffer* vertexBuffer = nullptr;
	ID3D11VertexShader* vertexShader = nullptr;
	ID3D11GeometryShader* geometryShader = nullptr;
	ID3D11PixelShader* pixelShader = nullptr;
	ID3D11InputLayout* vertexLayout = nullptr;
	ID3D11ShaderResourceView* textureView = nullptr;
	void Texture(string material, ID3D11Device* gDevice);
	void moveMesh(XMFLOAT3 position, ID3D11DeviceContext* gDeviceContext);

	vector<VertexData> vertices;
	XMFLOAT3 center;
};

#endif