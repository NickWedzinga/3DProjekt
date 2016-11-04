#pragma once
#ifndef HEIGHMAP_H
#define HEIGHMAP_H

#include "Mesh.h"
#include <stdio.h>



class Terrain : public Mesh
{
public:
	Terrain();
	~Terrain();

	void Initialize(ID3D11Device* gDevice);
	void Render(ID3D11DeviceContext* gDeviceContext);
	void LoadHeightMap();
	int getVertexCount();
	//void InitializeTerrainShaders(ID3D11Device* gDevice);
	float getHeightMapY(DirectX::XMFLOAT2 cord);

	//std::vector<VertexData> vecVertices;

	//ID3D11VertexShader* gVertexShaderT = nullptr;
	//ID3D11GeometryShader* gGeometryShaderT = nullptr;
	//ID3D11PixelShader* gPixelShaderT = nullptr;
	//ID3D11InputLayout* gVertexLayoutT = nullptr;

private:
	void InitializeBuffers(ID3D11Device* gDevice);
	void ShutDownBuffers();
	void RenderBuffers(ID3D11DeviceContext* gDeviceContext);
	void LoadTexture(ID3D11Device* gDevice);
	float* heightMap;

	int terrainWidth, terrainHeight;
	int vertexCount;
	ID3D11Buffer *vertexBuffer, *indexBuffer;
	float scaleFactor;
	
};

#endif