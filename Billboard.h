#pragma once
#ifndef BILLBOARD_H
#define BILLBOARD_H

#include "mesh.h"
class QuadTree;	//Because circular includes, forward declaration of QuadTree because QuadTree has billboard included.
class Camera;

class Billboard : public Mesh
{
public:
	Billboard(int ID);
	Billboard(int ID, QuadTree* quadTree);
	~Billboard();
	void Init(XMFLOAT3 camPos, ID3D11Device* gDevice);	//1
	void Render(ID3D11DeviceContext* gDeviceContext);	//1
	void Update(ID3D11DeviceContext* gDeviceContext);	//1
	void Update(ID3D11DeviceContext* gDeviceContext, Camera* camera);	//1
	void InitBBBuffer(ID3D11Device* gDevice);	//1
	vector<VertexData> used;					//1
private:
	XMFLOAT3 position;
	XMMATRIX rotationMatrix;
	ID3D11Buffer* bBBuffer = nullptr;
	float width, height;

	void InitShaders(ID3D11Device * gDevice);
};

#endif
