#pragma once
#ifndef MESH_H
#define MESH_H

#include "includes.h"
#include <d3d11.h>
#include <d3dcompiler.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")
#include <vector>

using namespace std;

class Mesh
{
protected:
	struct VertexData
	{
		XMFLOAT2 UV;
		XMFLOAT3 normal;
		XMFLOAT3 position;
	};

	ID3D11VertexShader* mVertexShader = nullptr;
	ID3D11GeometryShader* mGeometryShader = nullptr;
	ID3D11PixelShader* mPixelShader = nullptr;
	ID3D11InputLayout* mVertexLayout = nullptr;
	ID3D11Buffer *vertexBuffer = nullptr;
	ID3D11ShaderResourceView* textureView = nullptr;

	unsigned int ID;
	XMFLOAT3 worldPosition;
	string textureName;
	//ID3D11Buffer* VertexBuffer = nullptr;

public:
	vector<VertexData> vertices;

	void InitShaders(ID3D11Device* gDevice);
	void SetShaders(ID3D11DeviceContext* gDeviceContext);
};

#endif