#ifndef HEIGHMAP_H
#define HEIGHMAP_H

#include <d3d11.h>
#include <SimpleMath.h>
#include <SimpleMath.inl>
#include <stdio.h>



class Terrain
{
private:
	struct VertexData
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT4 color;
		
	};
	struct HeightMapCord
	{
		float x, y, z;
	};
public:
	Terrain();
	~Terrain();

	void Initialize(ID3D11Device* gDevice);
	void LoadHeightMap();
	void InitializeBuffers(ID3D11Device* gDevice);
	void RenderBuffers(ID3D11DeviceContext* gDeviceContext);
private:
	int terrainWidth, terrainHeight;
	int vertexCount, indexCount;
	HeightMapCord* heightMap;
	ID3D11Buffer *vertexBuffer, *indexBuffer;
};

#endif