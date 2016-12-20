#include "Terrain.h"

using namespace DirectX::SimpleMath;
using namespace DirectX; //Verkar som man kan ha fler än 1 using namespace, TIL.
using namespace std;

Terrain::Terrain(int ID)
{
	this->ID = ID;
	this->vertexBuffer = 0;
	this->indexBuffer = 0;
	this->terrainHeight = 0;
	this->terrainWidth = 0;
	this->scaleFactor = 1;
}

Terrain::~Terrain()
{
	indexBuffer->Release();
	delete heightMap;
}

void Terrain::Initialize(ID3D11Device* gDevice)
{
	LoadHeightMap();
	InitializeBuffers(gDevice);
	return;
}

void Terrain::Render(ID3D11DeviceContext *gDeviceContext)
{
	gDeviceContext->VSSetShader(vertexShader, nullptr, 0);
	gDeviceContext->HSSetShader(nullptr, nullptr, 0);
	gDeviceContext->DSSetShader(nullptr, nullptr, 0);
	gDeviceContext->GSSetShader(geometryShader, nullptr, 0);
	gDeviceContext->PSSetShader(pixelShader, nullptr, 0);

	gDeviceContext->PSSetShaderResources(0, 1, &textureView);
	RenderBuffers(gDeviceContext);
}

void Terrain::LoadHeightMap()
{
	char* filename = "Resources/Heightmaps/firstheightmap.bmp";
	FILE* filePtr;
	//int error;
	unsigned int count;
	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;
	int imageSize, i, j, k, index;
	unsigned char* bitmapImage;
	unsigned char height;

	fopen_s(&filePtr, filename, "rb");
	fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);

	terrainWidth = bitmapInfoHeader.biWidth;
	terrainHeight = bitmapInfoHeader.biHeight;

	imageSize = terrainWidth * terrainHeight;

	bitmapImage = new unsigned char[imageSize];

	//Move to the beginning of the BMP file
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	//Reading the data in the file
	count = fread(bitmapImage, 1, imageSize, filePtr);

	fclose(filePtr);
	
	
	
	heightMap = new HeightMapCord[terrainWidth*terrainHeight];
	k = 0;
	for (j = 0; j < terrainHeight; j++)
		for (i = 0; i < terrainWidth; i++)
		{
			index = (terrainHeight*j) + i;
			height = bitmapImage[k];
			heightMap[index].y = float(height)/ 8/*(256 / 2)*/;
			k += 1;
		}

	delete[] bitmapImage;
	bitmapImage = 0;
}

int Terrain::getVertexCount()
{
	return vertexCount;
}

void Terrain::InitializeTerrainShaders(ID3D11Device* gDevice)
{
	ID3D10Blob* pVS = nullptr;
	ID3D10Blob* pGS = nullptr;
	ID3D10Blob* pPS = nullptr;

	D3DCompileFromFile(L"Vertex.hlsl", nullptr, nullptr, "VS_main", "vs_5_0", 0, 0, &pVS, nullptr);
	D3DCompileFromFile(L"GeometryST.hlsl", nullptr, nullptr, "GS_main", "gs_5_0", 0, 0, &pGS, nullptr);
	D3DCompileFromFile(L"PixelST.hlsl", nullptr, nullptr, "PS_main", "ps_5_0", 0, 0, &pPS, nullptr);

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

float Terrain::getHeightMapY(DirectX::XMFLOAT2 cord)
{
	float height;
	int x = cord.x/ scaleFactor;
	//float decX = cord.x - x;
	int z = cord.y/ scaleFactor;
	//float decZ = cord.y - z;
	
	if (x < 0 || x > 256 || z < 0 || z > 256)
		height = -1;
	else
	{
		z *= 256;
		height = scaleFactor * heightMap[z + x].y;
	}
	return height;
}

void Terrain::InitializeBuffers(ID3D11Device* gDevice)
{
	int index, i, j;
	float leftX, rightX, upperZ, bottomZ;

	vertexCount = (terrainWidth - 1) * (terrainHeight - 1) * 6;

	index = 0;

	for (j = 0; j < terrainHeight - 1; j++)
	{
		for (i = 0; i < terrainWidth - 1; i++)
		{
			index = (terrainHeight*j) + i;
			VertexData temp;
			temp.normal = XMFLOAT3(0, 0, 0);
			temp.ID = this->ID;
			leftX = float(i);
			rightX = float(i + 1);
			upperZ = float(j + 1);
			bottomZ = float(j);



			//Triangle (1)
			//Upper left
			temp.position = scaleFactor*XMFLOAT3(leftX, this->heightMap[int((terrainHeight*upperZ) + leftX)].y, upperZ);
			temp.UV = XMFLOAT2(leftX / terrainWidth, (terrainHeight - upperZ) / terrainHeight);
			//temp.UV = XMFLOAT2(this->heightMap[int((terrainHeight*upperZ) + leftX)].y / 32, this->heightMap[int((terrainHeight*upperZ) + leftX)].y / 32);
			vertices.push_back(temp);
			index++;

			//Bottom right
			temp.position = scaleFactor*XMFLOAT3(rightX, this->heightMap[int((terrainHeight*bottomZ) + rightX)].y, bottomZ);
			temp.UV = XMFLOAT2(rightX / terrainWidth, (terrainHeight - bottomZ) / terrainHeight);
			//temp.UV = XMFLOAT2(this->heightMap[int((terrainHeight*bottomZ) + rightX)].y / 32, this->heightMap[int((terrainHeight*bottomZ) + rightX)].y / 32);
			vertices.push_back(temp);
			index++;

			//Bottom left
			temp.position = scaleFactor*XMFLOAT3(leftX, this->heightMap[int((terrainHeight*bottomZ) + leftX)].y, bottomZ);
			temp.UV = XMFLOAT2(leftX / terrainWidth, (terrainHeight - bottomZ) / terrainHeight);
			//temp.UV = XMFLOAT2(this->heightMap[int((terrainHeight*bottomZ) + leftX)].y / 32, this->heightMap[int((terrainHeight*bottomZ) + leftX)].y / 32);
			vertices.push_back(temp);
			index++;

			//Triangle (2)
			//Upper left
			temp.position = scaleFactor*XMFLOAT3(leftX, this->heightMap[int((terrainHeight*upperZ) + leftX)].y, upperZ);
			temp.UV = XMFLOAT2(leftX / terrainWidth, (terrainHeight - upperZ) / terrainHeight);
			//temp.UV = XMFLOAT2(this->heightMap[int((terrainHeight*upperZ) + leftX)].y / 32, this->heightMap[int((terrainHeight*upperZ) + leftX)].y / 32);
			vertices.push_back(temp);
			index++;

			//Upper right
			temp.position = scaleFactor*XMFLOAT3(rightX, this->heightMap[int((terrainHeight*upperZ) + rightX)].y, upperZ);
			temp.UV = XMFLOAT2(rightX / terrainWidth, (terrainHeight - upperZ) / terrainHeight);
			//temp.UV = XMFLOAT2(this->heightMap[int((terrainHeight*upperZ) + rightX)].y / 32, this->heightMap[int((terrainHeight*upperZ) + rightX)].y / 32);
			vertices.push_back(temp);
			index++;

			//Bottom right
			temp.position = scaleFactor*XMFLOAT3(rightX, this->heightMap[int((terrainHeight*bottomZ) + rightX)].y, bottomZ);
			temp.UV = XMFLOAT2(rightX / terrainWidth, (terrainHeight - bottomZ) / terrainHeight);
			//temp.UV = XMFLOAT2(this->heightMap[int((terrainHeight*bottomZ) + rightX)].y / 32, this->heightMap[int((terrainHeight*bottomZ) + rightX)].y / 32);
			vertices.push_back(temp);
			index++;
		}
	}

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

	return;
}

void Terrain::RenderBuffers(ID3D11DeviceContext *gDeviceContext)
{
	UINT32 stride;
	UINT32 offset;
	stride = sizeof(vertices[0]);
	offset = 0;

	gDeviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	gDeviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gDeviceContext->IASetInputLayout(vertexLayout);

	return;
}
