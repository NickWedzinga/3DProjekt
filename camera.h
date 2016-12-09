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
	void Update(MSG* msg, float heightY);
	void Init(ID3D11Device* gDevice);
	void CreatePlanes();


	XMFLOAT3 getPos();
	void setPos(XMFLOAT3 pos);
	XMFLOAT2 getMouse();
	void setMouse(XMFLOAT2 mouse);
	ID3D11Buffer* keyDataBuffer = nullptr;
	ID3D11Buffer* gWorldViewProjBuffer = nullptr;
	KEY_BUFFER keyData;
	XMVECTOR plane[6];
	CONSTANT_BUFFER cData;
	bool lockLight;

private:
	void CreateViewMatrix();
	void CreateConstantBuffer(ID3D11Device* gDevice);
	void initKeyBuffer(ID3D11Device* gDevice);
	void CreateWorldMatrix();
	void CreateProjectionMatrix();

	XMFLOAT3 pos;
	XMFLOAT2 mouse;
	XMFLOAT3 camDir;
	XMFLOAT2 arbitraryFloat2;
	XMVECTOR lockedLight;
	int flightMode;
	float moveSpeed;

};


#endif // !CAMERA_H