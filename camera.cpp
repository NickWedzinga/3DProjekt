#include "camera.h"

Camera::Camera()
{
	pos = XMFLOAT3(0, 0, 0);
	mouse = XMFLOAT2(0, 0);
	camDir = XMFLOAT3(0, 0, 0);
}

Camera::~Camera()
{
}

void Camera::Update(MSG* msg, XMMATRIX &view, XMVECTOR &camDirection, XMVECTOR &camPosition)
{
	float x, y;
	XMFLOAT3 rightf, forward;
	XMVECTOR rightv = XMVector3Cross(XMVECTOR(XMLoadFloat3(&XMFLOAT3(0, 1, 0))), XMVECTOR(XMLoadFloat3(&camDir)));
	XMStoreFloat3(&rightf, rightv);
	XMStoreFloat3(&forward, XMVector3Normalize(XMLoadFloat3(&XMFLOAT3(camDir.x, 0, camDir.z))));
	switch (msg->message)
	{
	case WM_MOUSEMOVE: //om muspekaren flyttats
		if ((x = (mouse.x - GET_X_LPARAM(msg->lParam))) != 0)
		{
			x = -x / (2 * XM_PI * 150);
			XMMATRIX rotation = XMMatrixRotationY(x);
			XMStoreFloat3(&camDir, (XMVector3Transform(XMVECTOR(XMLoadFloat3(&camDir)), rotation)));
			mouse.x = GET_X_LPARAM(msg->lParam);
			//mouse.x = 320;
		}
		if ((y = (mouse.y - GET_Y_LPARAM(msg->lParam))) != 0)
		{
			y = -y / (2 * XM_PI * 150);
			XMMATRIX rotation = XMMatrixRotationAxis(rightv, y);
			XMStoreFloat3(&camDir, (XMVector3Transform(XMVECTOR(XMLoadFloat3(&camDir)), rotation)));
			mouse.y = GET_Y_LPARAM(msg->lParam);
			//mouse.y = 240;
		}

	case WM_KEYDOWN:
		switch (msg->wParam)
		{
		case 0x57: //w
			pos.z += 0.15 * forward.z;
			pos.x += 0.15 * forward.x;
			break;
		case 0x41: //a
			pos.z -= 0.15 * rightf.z;
			pos.x -= 0.15 * rightf.x;
			break;
		case 0x53: //s
			pos.z -= 0.15 * forward.z;
			pos.x -= 0.15 * forward.x;
			break;
		case 0x44: //d
			pos.z += 0.15 * rightf.z;
			pos.x += 0.15 * rightf.x;
			break;
		case VK_ESCAPE: //esc
			msg->message = WM_QUIT;
			break;
		case VK_SPACE: //space
			pos.y += 0.15;
			break;
		case 0x58:  //x
			pos.y -= 0.15;
			break;
		}
		camPosition = XMLoadFloat3(&pos);
	}
	CreateViewMatrix(view, camDirection);
}

void Camera::Init(XMMATRIX &view, XMVECTOR &camDirection)
{
	pos = XMFLOAT3(0, 0, -10);
	mouse = XMFLOAT2(320, 240);
	camDir = XMFLOAT3(0, 0, 1);
	CreateViewMatrix(view, camDirection);
}

XMFLOAT3 Camera::getPos()
{
	return pos;
}

void Camera::setPos(XMFLOAT3 pos)
{
	this->pos = pos;
}

XMFLOAT2 Camera::getMouse()
{
	return mouse;
}

void Camera::setMouse(XMFLOAT2 mouse)
{
	this->mouse = mouse;
}

void Camera::CreateViewMatrix(XMMATRIX &ViewMatrix, XMVECTOR &camDirection)
{
	XMFLOAT3 cam1 = XMFLOAT3(pos.x, pos.y, pos.z);
	XMFLOAT3 at1 = XMFLOAT3(0, 1, 0);

	XMVECTOR cam = XMLoadFloat3(&cam1);
	XMVECTOR up = XMLoadFloat3(&at1);

	camDirection = XMLoadFloat3(&camDir);
	ViewMatrix = XMMatrixLookToLH(cam, camDirection, up);
}
