#pragma once
#ifndef MESH_H
#define MESH_H

#include "includes.h"

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
	
	unsigned int ID;
	XMFLOAT3 worldPosition;
	string textureName;
	//ID3D11Buffer* VertexBuffer = nullptr;

public:
	vector<VertexData> vertices;

};

#endif