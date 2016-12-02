#pragma once
#ifndef BILLBOARD_H
#define BILLBOARD_H

#include "mesh.h"

class Billboard : public Mesh
{
public:
	Billboard();
	~Billboard();
	void Init(XMFLOAT3 camPos, ID3D11Device* gDevice);
	void Render(ID3D11DeviceContext* gDeviceContext);
	void Update(XMFLOAT3 camPos, ID3D11DeviceContext* gDeviceContext);
	void InitBBBuffer(ID3D11Device* gDevice);
private:
	XMFLOAT3 position;
	XMMATRIX rotationMatrix;
	ID3D11Buffer* bBBuffer = nullptr;
	float width, height;
	void InitShaders(ID3D11Device * gDevice);
};

#endif
