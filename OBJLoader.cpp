#include "OBJLoader.h"
using namespace std;
using namespace DirectX::SimpleMath;
using namespace DirectX; //Verkar som man kan ha fler än 1 using namespace, TIL.


Object::Object(int ID)
{
	this->ID = ID;
}

Object::~Object()
{
	gMaterialBuffer->Release();
	sampler->Release();
	norTexView->Release();
}

void Object::LoadObject(ID3D11Device* gDevice)
{
	this->center = XMFLOAT3(0, 0, 0);
	string myFile("Resources/Objs/box.obj"),
		special,	//Take unwanted characters
		line,		//Contains any given line in the file
		mtl;		//Contains material info

	ifstream file(myFile);
	istringstream inputString;
	
	vector<XMFLOAT3> vertices1, vertices3;
	vector<XMFLOAT2> vertices2;
	XMFLOAT3 vtx1, vtx3;
	XMFLOAT2 vtx2;
	VertexData temp;
	temp.ID = this->ID;

	UINT valueV = 0;
	UINT valueVT = 0;
	UINT valueVN = 0;
	char valueSlash = 0;
	while (getline(file, line))
	{
		inputString.str(line);
		if (line.substr(0, 2) == "v ") //pos
		{
			inputString >> special >> vtx1.x >> vtx1.y >> vtx1.z;
			vertices1.push_back(vtx1);
		}
		else if (line.substr(0, 3) == "vt ") //uv
		{
			inputString >> special >> vtx2.x >> vtx2.y;
			vtx2 = XMFLOAT2(vtx2.x, 1 - vtx2.y);	//Because Maya uses different system
			vertices2.push_back(vtx2);
		}
		else if (line.substr(0, 3) == "vn ") //normal
		{
			inputString >> special >> vtx3.x >> vtx3.y >> vtx3.z;
			vertices3.push_back(vtx3);
		}
		else if (line.substr(0, 2) == "f ") //after f three values follow ex: 1/2/1, means first v line, 2nd vt line, first vn line
		{
			for (int j = 0; j < 3; j++)
			{
				if (j == 0)
				{
					inputString >> special >> valueV >> valueSlash >> valueVT >> valueSlash >> valueVN;
				}
				if (j == 1 || j == 2)
				{
					inputString >> valueV >> valueSlash >> valueVT >> valueSlash >> valueVN;
				}
				temp.position = vertices1.at(valueV - 1);

				if (valueVT != 0)
				{
					temp.UV = vertices2.at(valueVT - 1);
				}
				else if (valueVT == 0)
				{
					temp.UV = XMFLOAT2(0.0f, 0.0f);
				}
				temp.normal = vertices3.at(valueVN - 1);
				vertices.push_back(temp);
			}
		}
		else if (line.substr(0, 7) == "mtllib ")
		{
			inputString >> special >> mtl;
			MTLLoader(mtl, gDevice);
		}
		inputString.clear();

	}
	file.close();

	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(VertexData)*vertices.size();
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices.data();
	HRESULT hr = gDevice->CreateBuffer(&bufferDesc, &vertexData, &vertexBuffer);

	return;
}

void Object::materialCB(ID3D11Device * gDevice)
{
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(CONSTANT_BUFFER2);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &materialData;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	HRESULT hr = gDevice->CreateBuffer(&cbDesc, &InitData, &gMaterialBuffer);
}

void Object::MTLLoader(string mtlfile, ID3D11Device* gDevice)
{
	//mtlfile was sent from OBJLOADER, mtlfile == "box.mtl"
	string myfile(mtlfile), line, special, texture;
	XMFLOAT4 Kd, Ka, Ks;
	ifstream file(myfile);
	istringstream inputString2;

	while (getline(file, line))
	{
		inputString2.str(line);
		if (line.substr(0, 3) == "map")
		{
			inputString2 >> special >> texture; //material == "cube_box.jpg"

			Texture(texture, gDevice);
		}
		else if (line.substr(0, 3) == "Kd ") //En material, RGB Diffuse
		{
			inputString2 >> special >> Kd.x >> Kd.y >> Kd.z;
			materialData.KD = Kd;
		}
		else if (line.substr(0, 3) == "Ka ")
		{
			inputString2 >> special >> Ka.x >> Ka.y >> Ka.z;
			materialData.KA = Ka;
		}
		else if (line.substr(0, 3) == "Ks ")
		{
			inputString2 >> special >> Ks.x >> Ks.y >> Ks.z;
			materialData.KS = Ks;
		}
		inputString2.clear();
	}
	return;
}

void Object::InitializeObjectShaders(ID3D11Device* gDevice)
{
	ID3D10Blob* pVS = nullptr;
	ID3D10Blob* pGS = nullptr;
	ID3D10Blob* pPS = nullptr;

	D3DCompileFromFile(L"Vertex.hlsl", nullptr, nullptr, "VS_main", "vs_5_0", 0, 0, &pVS, nullptr);
	D3DCompileFromFile(L"Geometry.hlsl", nullptr, nullptr, "GS", "gs_5_0", 0, 0, &pGS, nullptr);
	D3DCompileFromFile(L"Fragment.hlsl", nullptr, nullptr, "PS_main", "ps_5_0", 0, 0, &pPS, nullptr);

	gDevice->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &vertexShader);
	gDevice->CreateGeometryShader(pGS->GetBufferPointer(), pGS->GetBufferSize(), nullptr, &geometryShader);
	gDevice->CreatePixelShader(pPS->GetBufferPointer(), pPS->GetBufferSize(), nullptr, &pixelShader);

	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "ID", 0, DXGI_FORMAT_R16_SINT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	gDevice->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), pVS->GetBufferPointer(), pVS->GetBufferSize(), &vertexLayout);


	pVS->Release();
	pGS->Release();
	pPS->Release();
	SetSampler(gDevice);
}

void Object::NormalTexture(string normal, ID3D11Device * gDevice)
{
	wchar_t nor[40];
	MultiByteToWideChar(CP_UTF8, 0, normal.c_str(), -1, nor, sizeof(nor) / sizeof(wchar_t));

	CreateWICTextureFromFile(gDevice, nor, NULL, &norTexView);
}

void Object::Render(ID3D11DeviceContext * gDeviceContext)
{
	gDeviceContext->VSSetShader(vertexShader, nullptr, 0);
	gDeviceContext->GSSetShader(geometryShader, nullptr, 0);
	gDeviceContext->PSSetShader(pixelShader, nullptr, 0);

	gDeviceContext->PSSetShaderResources(0, 1, &textureView); //Pipelina texturen
	gDeviceContext->PSSetShaderResources(1, 1, &norTexView);  //Pipelina normal-texturen

	UINT32 vertexSize = sizeof(vertices[0]);
	UINT32 offset = 0;

	gDeviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, &offset);

	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gDeviceContext->IASetInputLayout(vertexLayout);
}

void Object::SetSampler(ID3D11Device * gDevice)
{
	D3D11_SAMPLER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_SAMPLER_DESC));
	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.MinLOD = -FLT_MAX;
	desc.MaxLOD = FLT_MAX;
	desc.MipLODBias = 0.0f;
	desc.MaxAnisotropy = 1;
	desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	desc.BorderColor[0] = 1.0f;
	desc.BorderColor[1] = 1.0f;
	desc.BorderColor[2] = 1.0f;
	desc.BorderColor[3] = 1.0f;

	gDevice->CreateSamplerState(&desc, &sampler);
}

