#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include "includes.h"
#include "Lights.h"
#include <Windowsx.h>

class Camera
{
public:
	Camera();
	~Camera();
	void Update(MSG* msg, CONSTANT_BUFFER &cBuffer, float heightY, Lights &light);
	void Init(XMMATRIX &view, XMVECTOR &camDirection, Lights &light);
	void initKeyBuffer(ID3D11Device* gDevice);

	XMFLOAT3 getPos();
	void setPos(XMFLOAT3 pos);
	XMFLOAT2 getMouse();
	void setMouse(XMFLOAT2 mouse);
	ID3D11Buffer* keyDataBuffer = nullptr;
	KEY_BUFFER keyData;

private:
	void CreateViewMatrix(XMMATRIX &ViewMatrix, XMVECTOR &camDirection, Lights &light);
	XMFLOAT3 pos;
	XMFLOAT2 mouse;
	XMFLOAT3 camDir;
	XMFLOAT2 arbitraryFloat2;
	bool lockLight;
	XMVECTOR lockedLight;
	int flightMode;

};


#endif // !CAMERA_H