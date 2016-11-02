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

class mesh
{
protected:
	struct VertexData
	{
		XMFLOAT2 UV;
		XMFLOAT3 Normal;
		XMFLOAT3 position;
	};
	vector<VertexData> vertices;
	XMFLOAT3 worldPosition;
	string textureName;
	//ID3D11Buffer* VertexBuffer = nullptr;

public:


};

#endif
