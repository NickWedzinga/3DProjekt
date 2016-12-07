#include "camera.h"

Camera::Camera()
{
	pos = XMFLOAT3(0, 0, 0);
	mouse = XMFLOAT2(XPOS, YPOS);
	SetCursorPos(mouse.x, mouse.y);
	camDir = XMFLOAT3(0, 0, 0);
	lockLight = false;
	flightMode = 0;
	moveSpeed = 0.8f;
}

Camera::~Camera()
{
	keyDataBuffer->Release();;
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
				XMMATRIX rotation = XMMatrixRotationAxis(rightv, y);
				XMStoreFloat3(&camDir, (XMVector3Transform(XMVECTOR(XMLoadFloat3(&camDir)), rotation)));
				mouse.y = GET_Y_LPARAM(msg->lParam);
			}
			if (mouse.x > 3 * WIDTH / 4 || mouse.x < WIDTH / 4 || mouse.y > 3 * HEIGHT / 4 || mouse.y < HEIGHT / 4)
			{
				SetCursorPos(XPOS, YPOS);
				mouse = XMFLOAT2(WIDTH / 2, HEIGHT / 2);
			}
			break;
		case WM_KEYDOWN:
		{
			switch (msg->wParam)
			{
			case 0x57: //w
				if (GetAsyncKeyState(0x41)) //a
				{
					pos.z += moveSpeed * forward.z * cos(XM_PI / 4);
					pos.x += moveSpeed * forward.x * cos(XM_PI / 4);
					pos.z -= moveSpeed * rightf.z * cos(XM_PI / 4);
					pos.x -= moveSpeed * rightf.x * cos(XM_PI / 4);
				}
				else if (GetAsyncKeyState(0x44)) //d
				{
					pos.z += moveSpeed * forward.z * cos(XM_PI / 4);
					pos.x += moveSpeed * forward.x * cos(XM_PI / 4);
					pos.z += moveSpeed * rightf.z * cos(XM_PI / 4);
					pos.x += moveSpeed * rightf.x * cos(XM_PI / 4);
				}
				else
				{
					pos.z += moveSpeed * forward.z;
					pos.x += moveSpeed * forward.x;
				}
				if (GetAsyncKeyState(VK_SPACE)) // space
				{
					pos.y += moveSpeed;
				}
				else if (GetAsyncKeyState(0x60)) //z
				{
					pos.y -= moveSpeed;
				}
				break;
			case 0x41: //a
				if (GetAsyncKeyState(0x57)) //w
				{
					pos.z += moveSpeed * forward.z * cos(XM_PI / 4);
					pos.x += moveSpeed * forward.x * cos(XM_PI / 4);
					pos.z -= moveSpeed * rightf.z * cos(XM_PI / 4);
					pos.x -= moveSpeed * rightf.x * cos(XM_PI / 4);
				}
				else if (GetAsyncKeyState(0x53)) //s
				{
					pos.z -= moveSpeed * forward.z * cos(XM_PI / 4);
					pos.x -= moveSpeed * forward.x * cos(XM_PI / 4);
					pos.z -= moveSpeed * rightf.z * cos(XM_PI / 4);
					pos.x -= moveSpeed * rightf.x * cos(XM_PI / 4);
				}
				else
				{
					pos.z -= moveSpeed * rightf.z;
					pos.x -= moveSpeed * rightf.x;
				}
				if (GetAsyncKeyState(VK_SPACE)) // space
				{
					pos.y += moveSpeed;
				}
				else if (GetAsyncKeyState(0x60)) //z
				{
					pos.y -= moveSpeed;
				}
				break;
			case 0x53: //s
				if (GetAsyncKeyState(0x41)) //a
				{
					pos.z -= moveSpeed * forward.z * cos(XM_PI / 4);
					pos.x -= moveSpeed * forward.x * cos(XM_PI / 4);
					pos.z -= moveSpeed * rightf.z * cos(XM_PI / 4);
					pos.x -= moveSpeed * rightf.x * cos(XM_PI / 4);
				}
				else if (GetAsyncKeyState(0x44)) //d
				{
					pos.z -= moveSpeed * forward.z * cos(XM_PI / 4);
					pos.x -= moveSpeed * forward.x * cos(XM_PI / 4);
					pos.z += moveSpeed * rightf.z * cos(XM_PI / 4);
					pos.x += moveSpeed * rightf.x * cos(XM_PI / 4);
				}
				else
				{
					pos.z -= moveSpeed * forward.z;
					pos.x -= moveSpeed * forward.x;
				}
				if (GetAsyncKeyState(VK_SPACE)) // space
				{
					pos.y += moveSpeed;
				}
				else if (GetAsyncKeyState(0x60)) //z
				{
					pos.y -= moveSpeed;
				}
				break;
			case 0x44: //d
				if (GetAsyncKeyState(0x57)) //w
				{
					pos.z += moveSpeed * forward.z * cos(XM_PI / 4);
					pos.x += moveSpeed * forward.x * cos(XM_PI / 4);
					pos.z += moveSpeed * rightf.z * cos(XM_PI / 4);
					pos.x += moveSpeed * rightf.x * cos(XM_PI / 4);
				}
				else if (GetAsyncKeyState(0x53)) //s
				{
					pos.z -= moveSpeed * forward.z * cos(XM_PI / 4);
					pos.x -= moveSpeed * forward.x * cos(XM_PI / 4);
					pos.z += moveSpeed * rightf.z * cos(XM_PI / 4);
					pos.x += moveSpeed * rightf.x * cos(XM_PI / 4);
				}
				else
				{
					pos.z += moveSpeed * rightf.z;
					pos.x += moveSpeed * rightf.x;
				}
				if (GetAsyncKeyState(VK_SPACE)) // space
				{
					pos.y += moveSpeed;
				}
				else if (GetAsyncKeyState(0x60)) //z
				{
					pos.y -= moveSpeed;
				}
				break;
			case VK_ESCAPE: //esc
				msg->message = WM_QUIT;
				break;
			case VK_SPACE: //space
					pos.y += moveSpeed;
				break;
			case 0x5A:  //z
					pos.y -= moveSpeed;
				break;
			case 0x31:	//1	//Normalmap on
				this->keyData.normalMap = XMLoadFloat3(&XMFLOAT3(0, 0, 0));
				break;
			case 0x32:	//2	//Normalmap off
				this->keyData.normalMap = XMLoadFloat3(&XMFLOAT3(1, 0, 0));
				break;
			case 0x33:	//3	//Lock light off
				lockLight = false;
				break;
			case 0x34:	//4	//Lock light off
				lockLight = true;
				lockedLight = XMLoadFloat3(&camDir);
				break;
			case 0x35:	//5	//FlightMode off
				flightMode = 0;
				break;
			case 0x36:	//6	//FlightMode on
				flightMode = 1;
				break;
			break;
			}
		}
	}
	if (!lockLight)
		cBuffer.camPos = XMLoadFloat3(&pos);
	if (heightY != -1 && flightMode == 0)
		pos.y = heightY + 2;
	if (!lockLight)
		CreateViewMatrix(cBuffer.ViewMatrix, cBuffer.camDirection);
	else
		CreateViewMatrix(cBuffer.ViewMatrix, lockedLight);

}

void Camera::Init(XMMATRIX &view, XMVECTOR &camDirection)
{
	pos = XMFLOAT3(0, 2, -10);
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

void Camera::initKeyBuffer(ID3D11Device* gDevice)
{
	this->keyData.normalMap = XMLoadFloat3(&XMFLOAT3(0, 0, 0));

	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(KEY_BUFFER);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &keyData;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	gDevice->CreateBuffer(&cbDesc, &InitData, &keyDataBuffer);
}

void Camera::CreatePlanes(XMMATRIX &proj, XMMATRIX &view)
{
	XMMATRIX viewProjMatrix = view * proj;
	XMFLOAT4X4 viewProj;
	XMFLOAT4 temp[6];
	XMStoreFloat4x4(&viewProj, viewProjMatrix);

	temp[0].x = viewProj._14 + viewProj._11;
	temp[0].y = viewProj._24 + viewProj._21;
	temp[0].z = viewProj._34 + viewProj._31;
	temp[0].w = viewProj._44 + viewProj._41;

	temp[1].x = viewProj._14 - viewProj._11;
	temp[1].y = viewProj._24 - viewProj._21;
	temp[1].z = viewProj._34 - viewProj._31;
	temp[1].w = viewProj._44 - viewProj._41;

	temp[2].x = viewProj._14 - viewProj._12;
	temp[2].y = viewProj._24 - viewProj._22;
	temp[2].z = viewProj._34 - viewProj._32;
	temp[2].w = viewProj._44 - viewProj._42;

	temp[3].x = viewProj._14 + viewProj._12;
	temp[3].y = viewProj._24 + viewProj._22;
	temp[3].z = viewProj._34 + viewProj._32;
	temp[3].w = viewProj._44 + viewProj._42;

	temp[4].x = viewProj._13;
	temp[4].y = viewProj._23;
	temp[4].z = viewProj._33;
	temp[4].w = viewProj._43;

	temp[5].x = viewProj._14 - viewProj._13;
	temp[5].y = viewProj._24 - viewProj._23;
	temp[5].z = viewProj._34 - viewProj._33;
	temp[5].w = viewProj._44 - viewProj._43;

	

	for (int i = 0; i < 6; i++)
	{
		this->plane[i] = XMLoadFloat4(&temp[i]);
		this->plane[i] = XMVector4Normalize(this->plane[i]);
	}
}

void Camera::Culling()
{

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

void Camera::NormalizePlane()
{

}
