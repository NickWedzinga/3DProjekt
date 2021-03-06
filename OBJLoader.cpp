#include "OBJLoader.h"
using namespace std;
using namespace DirectX::SimpleMath;
using namespace DirectX; //Verkar som man kan ha fler �n 1 using namespace, TIL.


Object::Object()
{
}

Object::~Object()
{
}

void Object::LoadObject(ID3D11Device* gDevice)
{
	string myFile("box.obj"), special, line2, mtl;
	ifstream file(myFile);
	istringstream inputString;
	struct VertexV { float x, y, z; }; //skapar struct med x, y, z v�rden
	struct VertexVT { float u, v; };
	struct VertexVN { float x2, y2, z2; };

	vector<VertexV> vertices;
	vector<VertexVT> vertices2;
	vector<VertexVN> vertices3;
	VertexV vtx = { 0, 0, 0 };
	VertexVT vtx2 = { 0, 0 };
	VertexVN vtx3 = { 0, 0, 0 };
	
	int i = 0;
	UINT valueV = 0;
	UINT valueVT = 0;
	UINT valueVN = 0;
	char valueSlash = 0;
	while (getline(file, line2))
	{
		inputString.str(line2);
		if (line2.substr(0, 2) == "v ")
		{
			inputString >> special >> vtx.x >> vtx.y >> vtx.z;
			vertices.push_back(vtx);
		}
		else if (line2.substr(0, 3) == "vt ")
		{
			inputString >> special >> vtx2.u >> vtx2.v;
			vtx2.u = 1 - vtx2.u; //Because "Maya"
			vtx2.v = 1 - vtx2.v; //Because "Maya"
			vertices2.push_back(vtx2);
		}
		else if (line2.substr(0, 3) == "vn ")
		{
			inputString >> special >> vtx3.x2 >> vtx3.y2 >> vtx3.z2;
			vertices3.push_back(vtx3);
		}
		else if (line2.substr(0, 2) == "f ")
		{
			VertexData temp;
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
				temp.x = vertices.at(valueV - 1).x;
				temp.y = vertices.at(valueV - 1).y;
				temp.z = vertices.at(valueV - 1).z;

				if (valueVT != 0)
				{
					temp.u = vertices2.at(valueVT - 1).u;
					temp.v = vertices2.at(valueVT - 1).v;
				}
				else if (valueVT == 0)
				{
					temp.u = 0.0f;
					temp.v = 0.0f;
				}
				temp.x2 = vertices3.at(valueVN - 1).x2;
				temp.y2 = vertices3.at(valueVN - 1).y2;
				temp.z2 = vertices3.at(valueVN - 1).z2;
				//pushback
				triangleVertices.push_back(temp);
				i++;
			}
		}
		else if (line2.substr(0, 7) == "mtllib ")
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
	bufferDesc.ByteWidth = sizeof(VertexData)*triangleVertices.size();
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = triangleVertices.data();
	HRESULT hr = gDevice->CreateBuffer(&bufferDesc, &vertexData, &VertexBuffer);
   
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
	string myfile(mtlfile), line2, special, material;
	XMFLOAT4 Kd, Ka, Ks;
	ifstream file(myfile);
	istringstream inputString2;

	while (getline(file, line2))
	{
		inputString2.str(line2);
		if (line2.substr(0, 3) == "map")
		{
			inputString2 >> special >> material; //material == "cube_box.jpg"

			Texture(material, gDevice);
		}
		else if (line2.substr(0, 3) == "Kd ") //En material, RGB Diffuse
		{
			inputString2 >> special >> Kd.x >> Kd.y >> Kd.z;
			materialData.KD = Kd;
		}
		else if (line2.substr(0, 3) == "Ka ")
		{
			inputString2 >> special >> Ka.x >> Ka.y >> Ka.z;
			materialData.KA = Ka;
		}
		else if (line2.substr(0, 3) == "Ks ")
		{
			inputString2 >> special >> Ks.x >> Ks.y >> Ks.z;
			materialData.KS = Ks;
		}
		inputString2.clear();
	}
	return;
}

void Object::Texture(string material, ID3D11Device* gDevice)
{
	wchar_t mat[20];
	MultiByteToWideChar(CP_UTF8, 0, material.c_str(), -1, mat, sizeof(mat) / sizeof(wchar_t));

	CreateWICTextureFromFile(gDevice, mat, NULL, &textureView);
	return;
}
