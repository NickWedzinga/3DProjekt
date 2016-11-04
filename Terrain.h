#pragma once
#ifndef HEIGHMAP_H
#define HEIGHMAP_H

#include "mesh.h"
//#include <d3d11.h>
//#include <d3dcompiler.h>
#include <SimpleMath.h>
#include <SimpleMath.inl>
#include <stdio.h>
#include <vector>
//#pragma comment (lib, "d3d11.lib")
//#pragma comment (lib, "d3dcompiler.lib")


class Terrain : public mesh
{
public:
	Terrain();
	~Terrain();

	void Initialize(ID3D11Device* gDevice);
	void ShutDown();
	void Render(ID3D11DeviceContext* gDeviceContext);
	void LoadHeightMap();
	int getVertexCount();
	void InitializeTerrainShaders(ID3D11Device* gDevice);
	float getHeightMapY(DirectX::XMFLOAT2 cord);

	//std::vector<VertexData> vecVertices;

	ID3D11VertexShader* gVertexShaderT = nullptr;
	ID3D11GeometryShader* gGeometryShaderT = nullptr;
	ID3D11PixelShader* gPixelShaderT = nullptr;
	ID3D11InputLayout* gVertexLayoutT = nullptr;

private:
	void InitializeBuffers(ID3D11Device* gDevice);
	void ShutDownBuffers();
	void RenderBuffers(ID3D11DeviceContext* gDeviceContext);

	float* heightMap;

	int terrainWidth, terrainHeight;
	int vertexCount;
	ID3D11Buffer *vertexBuffer, *indexBuffer;
	float scaleFactor;
	
};

#endif