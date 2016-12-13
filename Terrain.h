#pragma once
#ifndef HEIGHMAP_H
#define HEIGHMAP_H

#include "mesh.h"
#include <stdio.h>

class Terrain : public Mesh
{
private:
	struct HeightMapCord
	{
		float x, y, z;
	};
public:
	Terrain(int ID);
	~Terrain();

	void Initialize(ID3D11Device* gDevice);
	void Render(ID3D11DeviceContext* gDeviceContext);
	void LoadHeightMap();
	int getVertexCount();
	void InitializeTerrainShaders(ID3D11Device* gDevice);
	float getHeightMapY(DirectX::XMFLOAT2 cord);

private:
	void InitializeBuffers(ID3D11Device* gDevice);
	void RenderBuffers(ID3D11DeviceContext* gDeviceContext);

	int terrainWidth, terrainHeight;
	int vertexCount;
	HeightMapCord* heightMap;
	ID3D11Buffer* indexBuffer;
	float scaleFactor;
	
};

#endif