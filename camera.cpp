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
	dontUpdate = false;
}

Camera::~Camera()
{
	keyDataBuffer->Release();
	gWorldViewProjBuffer->Release();
}

void Camera::Update(MSG* msg, float heightY)
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
			case 0x34:	//4	//Lock light on
				lockLight = true;
				lockedLight = XMLoadFloat3(&camDir);
				break;
			case 0x35:	//5	//FlightMode off
				flightMode = 0;
				break;
			case 0x36:	//6	//FlightMode on
				flightMode = 1;
				break;
			case 0x37:
				dontUpdate = false;
				break;
			case 0x38:
				dontUpdate = true;
				break;
			break;
			}
		}
	}
	if (!lockLight)
		cData.camPos = XMLoadFloat3(&pos);
	if (heightY != -1 && flightMode == 0)
		pos.y = heightY + 2;

	CreateViewMatrix();
}

void Camera::Init(ID3D11Device* gDevice)
{
	pos = XMFLOAT3(10, 10, -10);
	mouse = XMFLOAT2(320, 240);
	camDir = XMFLOAT3(0, 0, 1);

	CreateWorldMatrix();
	CreateViewMatrix();
	CreateProjectionMatrix();
	initKeyBuffer(gDevice);
	CreateConstantBuffer(gDevice);
	SetFrustumCoordinates();
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

void Camera::CreatePlanes()
{
	XMMATRIX view = cData.ViewMatrix;

	if (lockLight)
	{
		XMFLOAT3 upF = XMFLOAT3(0, 1, 0);	
		XMVECTOR up = XMLoadFloat3(&upF);
		cData.camDirection = XMVector3Normalize(lockedLight);
		view = XMMatrixLookToLH(cData.camPos, cData.camDirection, up);
	}

	XMMATRIX viewProjMatrix = view * cData.ProjMatrix;
	XMFLOAT4X4 viewProj;
	XMFLOAT4 temp[6];
	XMStoreFloat4x4(&viewProj, viewProjMatrix);

	//Left clipping plane
	temp[0].x = viewProj._14 + viewProj._11;
	temp[0].y = viewProj._24 + viewProj._21;
	temp[0].z = viewProj._34 + viewProj._31;
	temp[0].w = viewProj._44 + viewProj._41;

	//Right clipping plane
	temp[1].x = viewProj._14 - viewProj._11;
	temp[1].y = viewProj._24 - viewProj._21;
	temp[1].z = viewProj._34 - viewProj._31;
	temp[1].w = viewProj._44 - viewProj._41;

	//Near clipping plane
	temp[2].x = viewProj._13;
	temp[2].y = viewProj._23;
	temp[2].z = viewProj._33;
	temp[2].w = viewProj._43;

	//Far clipping plane
	temp[3].x = viewProj._14 - viewProj._13;
	temp[3].y = viewProj._24 - viewProj._23;
	temp[3].z = viewProj._34 - viewProj._33;
	temp[3].w = viewProj._44 - viewProj._43;

	//Top clipping plane
	temp[4].x = viewProj._14 - viewProj._12;
	temp[4].y = viewProj._24 - viewProj._22;
	temp[4].z = viewProj._34 - viewProj._32;
	temp[4].w = viewProj._44 - viewProj._42;

	//Bottom clipping plane
	temp[5].x = viewProj._14 + viewProj._12;
	temp[5].y = viewProj._24 + viewProj._22;
	temp[5].z = viewProj._34 + viewProj._32;
	temp[5].w = viewProj._44 + viewProj._42;

	

	for (int i = 0; i < 6; i++)
	{
		this->plane[i] = XMLoadFloat4(&temp[i]);
		this->plane[i] = XMVector4Normalize(this->plane[i]);
	}
}

void Camera::CreateWorldMatrix()
{
	cData.WorldMatrix = XMMatrixRotationRollPitchYaw(0, 0, 0);
}

void Camera::CreateProjectionMatrix()
{
	cData.ProjMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(FOV), WIDTH / HEIGHT, NEAR, FAR);
}

void Camera::getMinMaxCorners(XMFLOAT3 * input, XMFLOAT2 * output) //input is 4, output is 2
{
	output[0] = output[1] = XMFLOAT2(input[0].x, input[0].z);
	for (uint i = 0; i < 3; i++)
	{
		if (input[i + 1].x < output[0].x)
		{
			output[0].x = input[i + 1].x;
		}
		else if (input[i + 1].x > output[1].x)
		{
			output[1].x = input[i + 1].x;
		}
		if (input[i + 1].z < output[0].y)
		{
			output[0].y = input[i + 1].z;
		}
		else if (input[i + 1].z > output[1].y)
		{
			output[1].y = input[i + 1].z;
		}
	}
}

void Camera::SetFrustumCoordinates()
{
	XMFLOAT3 temp[8];
	float fovYhalf = FOV / 2;

	float halfHeightNear = NEAR * std::tan(fovYhalf);
	float halfWidthNear = (WIDTH / HEIGHT) * halfHeightNear;

	float halfHeightFar = (FAR * std::tan(fovYhalf));
	float halfWidthFar = (WIDTH / HEIGHT) * halfHeightFar;

	XMFLOAT3 nearOrigin = XMFLOAT3(0.0f, 0.0f, NEAR);
	XMFLOAT3 farOrigin = XMFLOAT3(0.0f, 0.0f, FAR);

	nearAndFarVertices[0] = XMLoadFloat3(&XMFLOAT3(-halfWidthNear, -halfHeightNear, nearOrigin.z));		//Top Right Near
	nearAndFarVertices[1] = XMLoadFloat3(&XMFLOAT3(halfWidthNear, -halfHeightNear, nearOrigin.z));		//Top Left Near
	nearAndFarVertices[2] = XMLoadFloat3(&XMFLOAT3(-halfWidthNear, halfHeightNear, nearOrigin.z));		//Bottom Right Near
	nearAndFarVertices[3] = XMLoadFloat3(&XMFLOAT3(halfWidthNear, halfHeightNear, nearOrigin.z));		//Bottom Left Near
	nearAndFarVertices[4] = XMLoadFloat3(&XMFLOAT3(-halfWidthFar, -halfHeightFar, farOrigin.z));		//Top Right Far
	nearAndFarVertices[5] = XMLoadFloat3(&XMFLOAT3(halfWidthFar, -halfHeightFar, farOrigin.z));			//Top Left Far
	nearAndFarVertices[6] = XMLoadFloat3(&XMFLOAT3(-halfWidthFar, halfHeightFar, farOrigin.z));			//Bottom Right Far
	nearAndFarVertices[7] = XMLoadFloat3(&XMFLOAT3(halfWidthFar, halfHeightFar, farOrigin.z));			//Bottom Left Far

	 XMMATRIX view = XMMatrixTranspose(cData.ViewMatrix);

	for (uint i = 0; i < 8; i++)
	{
		nearAndFarVertices[i] = XMVector3Transform(nearAndFarVertices[i], view);
		XMStoreFloat3(&temp[i], nearAndFarVertices[i]);
		nearAndFarVertices[i] += XMLoadFloat3(&pos);	
	}
}

void Camera::UpdateFrustumCoordinates()
{
	for (uint i = 0; i < 4; i++)
	{

	}
}

void Camera::CreateConstantBuffer(ID3D11Device* gDevice)
{
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(CONSTANT_BUFFER);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &cData;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	HRESULT hr = gDevice->CreateBuffer(&cbDesc, &InitData, &gWorldViewProjBuffer);
}

//bool Camera::rayPlaneIntersect(XMINT2* corners)
//{
//	bool output = false;
//	XMFLOAT3 nuCorners[4], planeCorners[4];
//	XMFLOAT2 rays[4], minMaxCorners[2];
//	XMFLOAT4 planeNormal[6];
//	XMFLOAT3 normDotOri, normDotRay;
//	XMVECTOR currentCorner, currentRay;
//	float t = 0;
//
//	for (uint i = 0; i < 6; ++i)
//	{
//		XMStoreFloat4(&planeNormal[i], plane[i]);
//	}
//	for (uint i = 0; i < 4; ++i)
//	{
//		nuCorners[i] = XMFLOAT3(corners[i].x, 0, corners[i].y);
//		rays[i] = XMFLOAT2(corners[(i + 1) % 4].x - corners[i].x, corners[(i + 1) % 4].y - corners[i].y);
//		
//		for (uint j = 0; j < 6; ++j)
//		{
//			currentCorner = XMLoadFloat3(&nuCorners[i]);
//			XMStoreFloat3(&normDotOri, XMVector3Dot(plane[j], currentCorner)); //FOR SOME REASON THIS FUCKING FUCK CAN'T RETURN A MOTHERFLIPPING FLOAT
//			currentRay = XMLoadFloat3(&XMFLOAT3(rays[i].x, 0, rays[i].y));
//			XMStoreFloat3(&normDotRay, XMVector3Dot(plane[j], currentRay));
//			t = (-planeNormal[j].w - normDotOri.x) / normDotRay.x;
//			if (j == 0)
//			{
//				XMStoreFloat3(&planeCorners[0], nearAndFarVertices[0]);
//				XMStoreFloat3(&planeCorners[1], nearAndFarVertices[2]);
//				XMStoreFloat3(&planeCorners[2], nearAndFarVertices[4]);
//				XMStoreFloat3(&planeCorners[3], nearAndFarVertices[6]);
//				getMinMaxCorners(planeCorners, minMaxCorners);
//			}
//			if (j == 1)
//			{
//				XMStoreFloat3(&planeCorners[0], nearAndFarVertices[1]);
//				XMStoreFloat3(&planeCorners[1], nearAndFarVertices[3]);
//				XMStoreFloat3(&planeCorners[2], nearAndFarVertices[5]);
//				XMStoreFloat3(&planeCorners[3], nearAndFarVertices[7]);
//				getMinMaxCorners(planeCorners, minMaxCorners);
//			}
//			if (j == 2)
//			{
//				XMStoreFloat3(&planeCorners[0], nearAndFarVertices[0]);
//				XMStoreFloat3(&planeCorners[1], nearAndFarVertices[1]);
//				XMStoreFloat3(&planeCorners[2], nearAndFarVertices[2]);
//				XMStoreFloat3(&planeCorners[3], nearAndFarVertices[3]);
//				getMinMaxCorners(planeCorners, minMaxCorners);
//			}
//			if (j == 3)
//			{
//				XMStoreFloat3(&planeCorners[0], nearAndFarVertices[4]);
//				XMStoreFloat3(&planeCorners[1], nearAndFarVertices[5]);
//				XMStoreFloat3(&planeCorners[2], nearAndFarVertices[6]);
//				XMStoreFloat3(&planeCorners[3], nearAndFarVertices[7]);
//				getMinMaxCorners(planeCorners, minMaxCorners);
//			}
//			if (j == 4)
//			{
//				XMStoreFloat3(&planeCorners[0], nearAndFarVertices[0]);
//				XMStoreFloat3(&planeCorners[1], nearAndFarVertices[1]);
//				XMStoreFloat3(&planeCorners[2], nearAndFarVertices[4]);
//				XMStoreFloat3(&planeCorners[3], nearAndFarVertices[5]);
//				getMinMaxCorners(planeCorners, minMaxCorners);
//			}
//			if (j == 5)
//			{
//				XMStoreFloat3(&planeCorners[0], nearAndFarVertices[2]);
//				XMStoreFloat3(&planeCorners[1], nearAndFarVertices[3]);
//				XMStoreFloat3(&planeCorners[2], nearAndFarVertices[6]);
//				XMStoreFloat3(&planeCorners[3], nearAndFarVertices[7]);
//				getMinMaxCorners(planeCorners, minMaxCorners);
//			}
//			if (t >= 0 && t <= 1)
//			{
//				XMFLOAT3 hitPoint;
//				XMStoreFloat3(&hitPoint, currentCorner + t * currentRay);
//				if (minMaxCorners[0].x <= hitPoint.x, minMaxCorners[1].x >= hitPoint.x, minMaxCorners[0].y <= hitPoint.z, minMaxCorners[1].y >= hitPoint.z)
//				{
//					output = true;
//					break;
//				}
//			}
//		}
//	}
//	return output;
//}

void Camera::CreateViewMatrix()
{
	XMFLOAT3 cam1 = XMFLOAT3(pos.x, pos.y, pos.z);
	XMFLOAT3 at1 = XMFLOAT3(0, 1, 0);

	XMVECTOR cam = XMLoadFloat3(&cam1);
	XMVECTOR up = XMLoadFloat3(&at1);
	cData.camDirection = XMVector3Normalize(XMLoadFloat3(&camDir));
	cData.ViewMatrix = XMMatrixLookToLH(cam, cData.camDirection, up);
}
