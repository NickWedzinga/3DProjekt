#include "Billboard.h"

Billboard::Billboard()
{
	position = XMFLOAT3(0, 5, 0);
	width = height = 3.0f;
}

Billboard::~Billboard()
{
}

void Billboard::Init(XMFLOAT3 camPos, ID3D11Device* gDevice)
{
	VertexData vertex;
	//matte!
	XMVECTOR camToBB = XMLoadFloat3(&position) - XMLoadFloat3(&camPos);
	camToBB = XMVector3Normalize(camToBB);
	XMVECTOR up = XMLoadFloat3(&XMFLOAT3(0, 1, 0));
	XMVECTOR left = XMVector3Cross(camToBB, up);
	up = XMVector3Cross(left, camToBB);
	XMStoreFloat3(&vertex.normal, -camToBB);

	//Upper left
	XMStoreFloat3(&vertex.position, XMLoadFloat3(&position) + left * width / 2 + up * height / 2);
	vertex.UV = XMFLOAT2(0, 0);
	vertices.push_back(vertex);
	//Bottom right
	XMStoreFloat3(&vertex.position, XMLoadFloat3(&position) - left * width / 2 - up * height / 2);
	vertex.UV = XMFLOAT2(1, 1);
	vertices.push_back(vertex);
	//Bottom left
	XMStoreFloat3(&vertex.position, XMLoadFloat3(&position) + left * width / 2 - up * height / 2);
	vertex.UV = XMFLOAT2(0, 1);
	vertices.push_back(vertex);

	//Upper left
	XMStoreFloat3(&vertex.position, XMLoadFloat3(&position) + left * width / 2 + up * height / 2);
	vertex.UV = XMFLOAT2(0, 0);
	vertices.push_back(vertex);
	//Upper right
	XMStoreFloat3(&vertex.position, XMLoadFloat3(&position) - left * width / 2 + up * height / 2);
	vertex.UV = XMFLOAT2(1, 0);
	vertices.push_back(vertex);
	//Bottom right
	XMStoreFloat3(&vertex.position, XMLoadFloat3(&position) - left * width / 2 - up * height / 2);
	vertex.UV = XMFLOAT2(1, 1);
	vertices.push_back(vertex);
	
	InitShaders(gDevice);

	D3D11_BUFFER_DESC vertexBufferDesc;
	memset(&vertexBufferDesc, 0, sizeof(vertexBufferDesc));
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexData)*vertices.size();
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices.data();
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	gDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);


	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(VertexData)*vertices.size();
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = vertices.data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	gDevice->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);
}

void Billboard::Render(ID3D11DeviceContext * gDeviceContext)
{
	UINT32 stride;
	UINT32 offset;

	stride = sizeof(vertices[0]);
	offset = 0;
	gDeviceContext->GSSetConstantBuffers(1, 1, &bBBuffer);
	gDeviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	gDeviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Billboard::Update(XMFLOAT3 camPos, ID3D11DeviceContext* gDeviceContext)
{
	XMFLOAT3 camToBB;
	camToBB.x = position.x - camPos.x;
	camToBB.y = position.y - camPos.y;
	camToBB.z = position.z - camPos.z;
	XMVECTOR normal = XMLoadFloat3(&camToBB);
	normal = XMVector3Normalize(-normal);
	float yaw, pitch, roll;
	XMFLOAT3 temp;
	XMStoreFloat3(&temp, XMVector3Dot(normal, XMLoadFloat3(&XMFLOAT3(0, 0, 1))));
	roll = temp.x;
	XMStoreFloat3(&temp, XMVector3Dot(normal, XMLoadFloat3(&XMFLOAT3(1, 0, 0))));
	pitch = temp.x;
	XMStoreFloat3(&temp, XMVector3Dot(normal, XMLoadFloat3(&XMFLOAT3(0, 1, 0))));
	yaw = temp.x;
	rotationMatrix = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&XMFLOAT3(pitch, yaw, roll)));	//According to msdn: 3D vector containing the Euler angles in the order pitch, then yaw, and then roll. Stupid programmers, the function is namned ...RollPitchYaw...  


	D3D11_MAPPED_SUBRESOURCE mappedResource;
	gDeviceContext->Map(bBBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, &rotationMatrix, sizeof(XMMATRIX));
	gDeviceContext->Unmap(bBBuffer, 0);
}

void Billboard::InitBBBuffer(ID3D11Device* gDevice)
{
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(XMMATRIX);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &rotationMatrix;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	gDevice->CreateBuffer(&cbDesc, &InitData, &bBBuffer);
}



void Billboard::InitShaders(ID3D11Device * gDevice)
{
	ID3D10Blob* pVS = nullptr;
	ID3D10Blob* pGS = nullptr;
	ID3D10Blob* pPS = nullptr;

	D3DCompileFromFile(L"VertexST.hlsl", nullptr, nullptr, "VS_main", "vs_4_0", 0, 0, &pVS, nullptr);
	D3DCompileFromFile(L"GeometryBB.hlsl", nullptr, nullptr, "GS_main", "gs_4_0", 0, 0, &pGS, nullptr);
	D3DCompileFromFile(L"PixelST.hlsl", nullptr, nullptr, "PS_main", "ps_4_0", 0, 0, &pPS, nullptr);

	gDevice->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &vertexShader);
	gDevice->CreateGeometryShader(pGS->GetBufferPointer(), pGS->GetBufferSize(), nullptr, &geometryShader);
	gDevice->CreatePixelShader(pPS->GetBufferPointer(), pPS->GetBufferSize(), nullptr, &pixelShader);

	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "ID", 0, DXGI_FORMAT_R16_SINT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	gDevice->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), pVS->GetBufferPointer(), pVS->GetBufferSize(), &vertexLayout);


	pVS->Release();
	pGS->Release();
	pPS->Release();
}
