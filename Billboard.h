#pragma once
#ifndef BILLBOARD_H
#define BILLBOARD_H

#include "mesh.h"
class QuadTree;	//Because circular includes, forward declaration of QuadTree because QuadTree has billboard included.
class Camera;

class Billboard : public Mesh
{
public:
	Billboard(int ID, QuadTree* quadTree);
	~Billboard();
	void Init(XMFLOAT3 camPos, ID3D11Device* gDevice);
	void Render(ID3D11DeviceContext* gDeviceContext);
	void Update(ID3D11DeviceContext* gDeviceContext);
	vector<VertexData> used;
private:
	XMFLOAT3 position;
	float width, height;

	void InitShaders(ID3D11Device * gDevice);
};

#endif
