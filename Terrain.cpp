#include "Terrain.h"

using namespace DirectX::SimpleMath;
using namespace DirectX; //Verkar som man kan ha fler �n 1 using namespace, TIL.
using namespace std;

Terrain::Terrain()
{
	this->vertexBuffer = 0;
	this->indexBuffer = 0;
	this->terrainHeight = 0;
	this->terrainWidth = 0;
	this->scaleFactor = 0.4;
}

Terrain::~Terrain()
{
	delete[] heightMap;
}

void Terrain::Initialize(ID3D11Device* gDevice)
{
	LoadHeightMap();
	InitializeBuffers(gDevice);
	return;
}

void Terrain::ShutDown()
{
	ShutDownBuffers();
	return;
}

void Terrain::Render(ID3D11DeviceContext *gDeviceContext)
{
	RenderBuffers(gDeviceContext);
	return;
}

void Terrain::LoadHeightMap()
{
	char* filename = "objs/firstheightmap.bmp";
	FILE* filePtr;
	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;
	int imageSize, i, j, k;
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
	fread(bitmapImage, 1, imageSize, filePtr);

	fclose(filePtr);
	
	
	
	heightMap = new float[terrainWidth*terrainHeight];
	k = 0;
	for (j = 0; j < terrainHeight; j++)
		for (i = 0; i < terrainWidth; i++)
		{
			height = bitmapImage[k];
			heightMap[k] = float(height)/ 8;
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

	D3DCompileFromFile(L"VertexST.hlsl", nullptr, nullptr, "VS_main", "vs_4_0", 0, 0, &pVS, nullptr);
	D3DCompileFromFile(L"GeometryST.hlsl", nullptr, nullptr, "GS_main", "gs_4_0", 0, 0, &pGS, nullptr);
	D3DCompileFromFile(L"PixelST.hlsl", nullptr, nullptr, "PS_main", "ps_4_0", 0, 0, &pPS, nullptr);

	gDevice->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &gVertexShaderT);
	gDevice->CreateGeometryShader(pGS->GetBufferPointer(), pGS->GetBufferSize(), nullptr, &gGeometryShaderT);
	gDevice->CreatePixelShader(pPS->GetBufferPointer(), pPS->GetBufferSize(), nullptr, &gPixelShaderT);

	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	gDevice->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), pVS->GetBufferPointer(), pVS->GetBufferSize(), &gVertexLayoutT);


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
		height = scaleFactor * heightMap[z + x];
	}
	return height;
}

void Terrain::InitializeBuffers(ID3D11Device* gDevice)
{
	int index, i, j;
	float leftX, rightX, upperZ, bottomZ;
	float upperLeftY, upperRightY, bottomLeftY, bottomRightY;
	//int index1, index2, index3, index4;		//Array? int index[4];

	vertexCount = (terrainWidth - 1) * (terrainHeight - 1) * 6;
	indexCount = vertexCount;

	index = 0;

	for (j = 0; j < terrainHeight - 1; j++)
	{
		for (i = 0; i < terrainWidth - 1; i++)
		{
			index = (terrainHeight*j) + i;
			VertexData temp;
			leftX = float(i);
			rightX = float(i + 1);
			upperZ = float(j + 1);
			bottomZ = float(j);

			upperLeftY = this->heightMap[int((terrainHeight*upperZ) + leftX)];
			upperRightY = this->heightMap[int((terrainHeight*upperZ) + rightX)];
			bottomLeftY = this->heightMap[int((terrainHeight*bottomZ) + leftX)];
			bottomRightY = this->heightMap[int((terrainHeight*bottomZ) + rightX)];

			//Triangle (1)
			//Upper left
			temp.pos = scaleFactor*XMFLOAT3(leftX, upperLeftY, upperZ);
			temp.color = XMFLOAT4(0.0f, upperLeftY / 32, 0.0f, 1.0f);
			vecVertices.push_back(temp);
			index++;

			//Bottom right
			temp.pos = scaleFactor*XMFLOAT3(rightX, bottomRightY, bottomZ);
			temp.color = XMFLOAT4(0.0f, bottomRightY / 32, 0.0f, 1.0f);
			vecVertices.push_back(temp);
			index++;

			//Bottom left
			temp.pos = scaleFactor*XMFLOAT3(leftX, bottomLeftY, bottomZ);
			temp.color = XMFLOAT4(0.0f, bottomLeftY / 32, 0.0f, 1.0f);
			vecVertices.push_back(temp);
			index++;

			//Triangle (2)
			//Upper left
			temp.pos = scaleFactor*XMFLOAT3(leftX, upperLeftY, upperZ);
			temp.color = XMFLOAT4(0.0f, upperLeftY / 32, 0.0f, 1.0f);
			vecVertices.push_back(temp);
			index++;

			//Upper right
			temp.pos = scaleFactor*XMFLOAT3(rightX, upperRightY, upperZ);
			temp.color = XMFLOAT4(0.0f, upperRightY / 32, 0.0f, 1.0f);
			vecVertices.push_back(temp);
			index++;

			//Bottom right
			temp.pos = scaleFactor*XMFLOAT3(rightX, bottomRightY, bottomZ);
			temp.color = XMFLOAT4(0.0f, bottomRightY / 32, 0.0f, 1.0f);
			vecVertices.push_back(temp);
			index++;
		}
	}

	D3D11_BUFFER_DESC vertexBufferDesc;
	memset(&vertexBufferDesc, 0, sizeof(vertexBufferDesc));
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexData)*vecVertices.size();
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vecVertices.data();
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	gDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);


	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(VertexData)*vecVertices.size();
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = vecVertices.data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	gDevice->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);

	return;
}

void Terrain::ShutDownBuffers()
{
	if (indexBuffer)
	{
		indexBuffer->Release();
		indexBuffer = 0;
	}

	if (vertexBuffer)
	{
		vertexBuffer->Release();
		vertexBuffer = 0;
	}
	return;
}

void Terrain::RenderBuffers(ID3D11DeviceContext *gDeviceContext)
{
	UINT32 stride;
	UINT32 offset;

	//stride = sizeof(VertexData);
	//stride = sizeof(vertices[0]);
	stride = sizeof(vecVertices[0]);
	offset = 0;

	gDeviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	gDeviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gDeviceContext->IASetInputLayout(gVertexLayoutT);

	return;
}
