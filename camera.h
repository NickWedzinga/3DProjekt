#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include "includes.h"
#include "QuadTree.h"
#include <Windowsx.h>

class Camera
{
public:
	Camera();
	~Camera();
	void Update(MSG* msg, CONSTANT_BUFFER &cBuffer, float heightY);
	void Init(XMMATRIX &view, XMVECTOR &camDirection);
	void initKeyBuffer(ID3D11Device* gDevice);
	void CreatePlanes(XMMATRIX &proj, XMMATRIX &view);


	XMFLOAT3 getPos();
	void setPos(XMFLOAT3 pos);
	XMFLOAT2 getMouse();
	void setMouse(XMFLOAT2 mouse);
	ID3D11Buffer* keyDataBuffer = nullptr;
	KEY_BUFFER keyData;
	XMVECTOR plane[6];

private:
	void CreateViewMatrix(XMMATRIX &ViewMatrix, XMVECTOR &camDirection);

	XMFLOAT3 pos;
	XMFLOAT2 mouse;
	XMFLOAT3 camDir;
	XMFLOAT2 arbitraryFloat2;
	bool lockLight;
	XMVECTOR lockedLight;
	int flightMode;
	float moveSpeed;

};


#endif // !CAMERA_H