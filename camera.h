#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include "includes.h"

#include <windows.h>
#include <Windowsx.h>

using namespace DirectX;

class Camera
{
public:
	Camera();
	~Camera();
	void Update(MSG* msg, CONSTANT_BUFFER &cBuffer, float heightY);

	void Init(XMMATRIX &view, XMVECTOR &camDirection);

	XMFLOAT3 getPos();
	void setPos(XMFLOAT3 pos);
	XMFLOAT2 getMouse();
	void setMouse(XMFLOAT2 mouse);

private:
	void CreateViewMatrix(XMMATRIX &ViewMatrix, XMVECTOR &camDirection);

	XMFLOAT3 pos;
	XMFLOAT2 mouse;
	XMFLOAT3 camDir;
	XMFLOAT2 arbitraryFloat2;

};


#endif // !CAMERA_H