#ifndef CAMERA_H
#define CAMERA_H

#include <windows.h>
#include <Windowsx.h>
#include <DirectXMath.h>
#include <d3d11.h>
#pragma comment (lib, "d3d11.lib")

using namespace DirectX;

class Camera
{
public:
	Camera();
	~Camera();
	void Update(MSG* msg, XMMATRIX &view, XMVECTOR &camDirection);

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

};


#endif // !CAMERA_H