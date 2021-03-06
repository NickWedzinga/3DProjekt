#include "camera.h"

Camera::Camera()
{
	pos = XMFLOAT3(0, 0, 0);
	mouse = XMFLOAT2(XPOS, YPOS);
	SetCursorPos(mouse.x, mouse.y);
	camDir = XMFLOAT3(0, 0, 0);
	arbitraryFloat2 = XMFLOAT2(0, 0);
}

Camera::~Camera()
{
}

void Camera::Update(MSG* msg, CONSTANT_BUFFER &cBuffer, float heightY)
{
	float x, y;
	XMFLOAT3 rightf, forward;
	XMVECTOR rightv = XMVector3Cross(XMVECTOR(XMLoadFloat3(&XMFLOAT3(0, 1, 0))), XMVECTOR(XMLoadFloat3(&camDir)));
	XMStoreFloat3(&rightf, rightv);
	XMStoreFloat3(&forward, XMVector3Normalize(XMLoadFloat3(&XMFLOAT3(camDir.x, 0, camDir.z))));
	switch (msg->message)
	{
	case WM_MOUSEMOVE: //om muspekaren flyttats
		if ((x = (mouse.x - GET_X_LPARAM(float(msg->lParam)))) != 0)
		{
			x = -x / 3750; //(1 / 3750) is an arbitrary scalar to slow down camera swivel speed
			XMMATRIX rotation = XMMatrixRotationY(x);
			XMStoreFloat3(&camDir, (XMVector3Transform(XMVECTOR(XMLoadFloat3(&camDir)), rotation)));
			mouse.x = GET_X_LPARAM(msg->lParam);
		}
		if ((y = (mouse.y - GET_Y_LPARAM(msg->lParam))) != 0)
		{
			y = -y / 5000; //(1 / 5000) is an arbitrary scalar to slow down camera swivel speed
			arbitraryFloat2.y += y;
			XMMATRIX rotation = XMMatrixRotationAxis(rightv, y);
			XMStoreFloat3(&camDir, (XMVector3Transform(XMVECTOR(XMLoadFloat3(&camDir)), rotation)));
			mouse.y = GET_Y_LPARAM(msg->lParam);
		}
		if (mouse.x > 3 * WIDTH / 4 || mouse.x < WIDTH / 4 || mouse.y > 3 * HEIGHT / 4 || mouse.y < HEIGHT / 4)
		{
			SetCursorPos(XPOS, YPOS);
			mouse = XMFLOAT2(WIDTH / 2, HEIGHT / 2);
		}
	case WM_KEYDOWN:
		switch (msg->wParam)
		{
		case 0x57: //w
			if (GetAsyncKeyState(0x41)) //a
			{
				pos.z += 0.15 * forward.z * cos(XM_PI/4);
				pos.x += 0.15 * forward.x * cos(XM_PI / 4);
				pos.z -= 0.15 * rightf.z * cos(XM_PI / 4);
				pos.x -= 0.15 * rightf.x * cos(XM_PI / 4);
			}
			else if (GetAsyncKeyState(0x44)) //d
			{
				pos.z += 0.15 * forward.z * cos(XM_PI / 4);
				pos.x += 0.15 * forward.x * cos(XM_PI / 4);
				pos.z += 0.15 * rightf.z * cos(XM_PI / 4);
				pos.x += 0.15 * rightf.x * cos(XM_PI / 4);
			}
			else
			{
				pos.z += 0.15 * forward.z;
				pos.x += 0.15 * forward.x;
			}
			if (GetAsyncKeyState(VK_SPACE) && heightY == -1) // space
			{
				pos.y += 0.15;
			}
			else if (GetAsyncKeyState(0x60) && heightY == -1) //z
			{
				pos.y -= 0.15;
			}
			break;
		case 0x41: //a
			if (GetAsyncKeyState(0x57)) //w
			{
				pos.z += 0.15 * forward.z * cos(XM_PI / 4);
				pos.x += 0.15 * forward.x * cos(XM_PI / 4);
				pos.z -= 0.15 * rightf.z * cos(XM_PI / 4);
				pos.x -= 0.15 * rightf.x * cos(XM_PI / 4);
			}
			else if (GetAsyncKeyState(0x53)) //s
			{
				pos.z -= 0.15 * forward.z * cos(XM_PI / 4);
				pos.x -= 0.15 * forward.x * cos(XM_PI / 4);
				pos.z -= 0.15 * rightf.z * cos(XM_PI / 4);
				pos.x -= 0.15 * rightf.x * cos(XM_PI / 4);
			}
			else
			{
				pos.z -= 0.15 * rightf.z;
				pos.x -= 0.15 * rightf.x;
			}
			if (GetAsyncKeyState(VK_SPACE) && heightY == -1) // space
			{
				pos.y += 0.15;
			}
			else if (GetAsyncKeyState(0x60) && heightY == -1) //z
			{
				pos.y -= 0.15;
			}
			break;
		case 0x53: //s
			if (GetAsyncKeyState(0x41)) //a
			{
				pos.z -= 0.15 * forward.z * cos(XM_PI / 4);
				pos.x -= 0.15 * forward.x * cos(XM_PI / 4);
				pos.z -= 0.15 * rightf.z * cos(XM_PI / 4);
				pos.x -= 0.15 * rightf.x * cos(XM_PI / 4);
			}
			else if (GetAsyncKeyState(0x44)) //d
			{
				pos.z -= 0.15 * forward.z * cos(XM_PI / 4);
				pos.x -= 0.15 * forward.x * cos(XM_PI / 4);
				pos.z += 0.15 * rightf.z * cos(XM_PI / 4);
				pos.x += 0.15 * rightf.x * cos(XM_PI / 4);
			}
			else
			{
				pos.z -= 0.15 * forward.z;
				pos.x -= 0.15 * forward.x;
			}
			if (GetAsyncKeyState(VK_SPACE) && heightY == -1) // space
			{
				pos.y += 0.15;
			}
			else if (GetAsyncKeyState(0x60) && heightY == -1) //z
			{
				pos.y -= 0.15;
			}
			break;
		case 0x44: //d
			if (GetAsyncKeyState(0x57)) //w
			{
				pos.z += 0.15 * forward.z * cos(XM_PI / 4);
				pos.x += 0.15 * forward.x * cos(XM_PI / 4);
				pos.z += 0.15 * rightf.z * cos(XM_PI / 4);
				pos.x += 0.15 * rightf.x * cos(XM_PI / 4);
			}
			else if (GetAsyncKeyState(0x53)) //s
			{
				pos.z -= 0.15 * forward.z * cos(XM_PI / 4);
				pos.x -= 0.15 * forward.x * cos(XM_PI / 4);
				pos.z += 0.15 * rightf.z * cos(XM_PI / 4);
				pos.x += 0.15 * rightf.x * cos(XM_PI / 4);
			}
			else
			{
				pos.z += 0.15 * rightf.z;
				pos.x += 0.15 * rightf.x;
			}
			if (GetAsyncKeyState(VK_SPACE) && heightY == -1) // space
			{
				pos.y += 0.15;
			}
			else if (GetAsyncKeyState(0x60) && heightY == -1) //z
			{
				pos.y -= 0.15;
			}
			break;
		case VK_ESCAPE: //esc
			msg->message = WM_QUIT;
			break;
		case VK_SPACE: //space
			if(heightY == -1)
				pos.y += 0.15;
			break;
		case 0x5A:  //z
			if(heightY == -1)
				pos.y -= 0.15;
			break;
		
		}
		cBuffer.camPosition = XMLoadFloat3(&pos);
	}
	if (heightY != -1)
		pos.y = heightY;
	CreateViewMatrix(cBuffer.ViewMatrix, cBuffer.camDirection);
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
	XMFLOAT3 cam1 = XMFLOAT3(pos.x, pos.y+2, pos.z);
	XMFLOAT3 at1 = XMFLOAT3(0, 1, 0);

	XMVECTOR cam = XMLoadFloat3(&cam1);
	XMVECTOR up = XMLoadFloat3(&at1);

	camDirection = XMLoadFloat3(&camDir);
	ViewMatrix = XMMatrixLookToLH(cam, camDirection, up);
}
