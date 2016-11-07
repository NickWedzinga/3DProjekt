#include "Billboard.h"

Billboard::Billboard()
{
	width = height = 0.0f;
}

Billboard::~Billboard()
{
}

vector<Mesh::VertexData> Billboard::makeQuad(XMFLOAT3 position, XMFLOAT3 camPos)
{
	vector<VertexData> quad;
	VertexData vertex;
	//matte!
	XMVECTOR camToBB = XMLoadFloat3(&position) - XMLoadFloat3(&camPos);
	XMVECTOR up = XMLoadFloat3(&XMFLOAT3(0, 1, 0));
	XMVECTOR left = XMVector3Cross(up, camToBB);
	up = XMVector3Cross(left, camToBB);
	XMStoreFloat3(&vertex.normal, -camToBB);

	//Upper left
	XMStoreFloat3(&vertex.position, left * width / 2 + up * height / 2);
	vertex.UV = XMFLOAT2(0, 0);
	quad.push_back(vertex);
	//Bottom right
	XMStoreFloat3(&vertex.position, -left * width / 2 - up * height / 2);
	vertex.UV = XMFLOAT2(1, 1);
	quad.push_back(vertex);
	//Bottom left
	XMStoreFloat3(&vertex.position, left * width / 2 - up * height / 2);
	vertex.UV = XMFLOAT2(0, 1);
	quad.push_back(vertex);

	//Upper left
	XMStoreFloat3(&vertex.position, left * width / 2 + up * height / 2);
	vertex.UV = XMFLOAT2(0, 0);
	quad.push_back(vertex);
	//Upper right
	XMStoreFloat3(&vertex.position, - left * width / 2 + up * height / 2);
	vertex.UV = XMFLOAT2(1, 0);
	quad.push_back(vertex);
	//Bottom right
	XMStoreFloat3(&vertex.position, -left * width / 2 - up * height / 2);
	vertex.UV = XMFLOAT2(1, 1);
	quad.push_back(vertex);

	return quad;
}

void Billboard::makeTexture(const string & text, ID3D11Device* gDevice) //returnade bool innan, satte void så länge för att matcha .h
{
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	gDevice->CreateTexture2D(&textureDesc, NULL, &texture);
}
