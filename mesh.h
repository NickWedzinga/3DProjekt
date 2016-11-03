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
		XMFLOAT3 normal;
		XMFLOAT3 position;
	};
	
	unsigned int ID;
	XMFLOAT3 worldPosition;
	string textureName;
	//ID3D11Buffer* VertexBuffer = nullptr;

public:
	vector<VertexData> vertices;

};

#endif