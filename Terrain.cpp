#include "Terrain.h"

using namespace DirectX::SimpleMath;
using namespace DirectX; //Verkar som man kan ha fler än 1 using namespace, TIL.
using namespace std;

Terrain::Terrain()
{
	vertexBuffer = 0;
	indexBuffer = 0;
}

Terrain::~Terrain()
{
}

void Terrain::Initialize(ID3D11Device* gDevice)
{
	terrainWidth = 100;
	terrainHeight = 100;

	//LoadHeightMap();
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
	char* filename = "../objs/heightmap.bmp";
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

	//terrainWidth = bitmapInfoHeader.biWidth;
	//terrainHeight = bitmapInfoHeader.biHeight;

	imageSize = terrainWidth * terrainHeight * 3;

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
			height = bitmapImage[k];
			index = (terrainHeight*j) + i;
			heightMap[index].x = (float)i;
			heightMap[index].y = (float)height;
			heightMap[index].z = (float)j;
			k += 3;			//We only need one color from the bmp so we take three steps so we skipp green and blue
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

void Terrain::InitializeBuffers(ID3D11Device* gDevice)
{
	
	unsigned long* indices;
	int index, i, j;
	float leftX, rightX, upperZ, bottomZ;
	//int index1, index2, index3, index4;		//Array? int index[4];

	vertexCount = (terrainWidth - 1) * (terrainHeight - 1) * 6;
	//vertexCount = 8 * 8 * 6;
	indexCount = vertexCount;

	indices = new unsigned long[indexCount];

	index = 0;

	/*for (j = 0; j < (terrainHeight - 1); j++)
		for (i = 0; i < (terrainWidth - 1); i++)
		{
			//Line 1
			//Upper Left
			posX = (float)i;
			posZ = (float)(j + 1);

			vertices[index].pos = XMFLOAT3(posX, 0.0f, posZ);
			vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			index++;

			//Upper right
			posX = (float)(i + 1);
			posZ = (float)(j + 1);

			vertices[index].pos = XMFLOAT3(posX, 0.0f, posZ);
			vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			index++;

			//Line 2
			//Upper right
			posX = (float)(i + 1);
			posZ = (float)(j + 1);

			vertices[index].pos = XMFLOAT3(posX, 0.0f, posZ);
			vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			index++;

			//Bottom right
			posX = (float)(i + 1);
			posZ = (float)j;

			vertices[index].pos = XMFLOAT3(posX, 0.0f, posZ);
			vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			index++;

			//Line 3
			//Bottom right
			posX = (float)(i + 1);
			posZ = (float)j;

			vertices[index].pos = XMFLOAT3(posX, 0.0f, posZ);
			vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			index++;

			//Bottom left
			posX = (float)i;
			posZ = (float)j;

			vertices[index].pos = XMFLOAT3(posX, 0.0f, posZ);
			vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			index++;

			//Line 4
			//Bottom left
			posX = (float)i;
			posZ = (float)j;

			vertices[index].pos = XMFLOAT3(posX, 0.0f, posZ);
			vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			index++;

			//Upper left
			posX = (float)i;
			posZ = (float)(j + 1);

			vertices[index].pos = XMFLOAT3(posX, 0.0f, posZ);
			vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			index++;
		}*/


	for (j = 0; j < terrainHeight - 1; j++)
	{
		for (i = 0; i < terrainWidth - 1; i++)
		{
			VertexData temp;
			leftX = float(i);
			rightX = float(i + 1);
			upperZ = float(j + 1);
			bottomZ = float(j);

			//Triangle (1)
			//Upper left
			temp.pos = XMFLOAT3(leftX, 0.0f, upperZ);
			temp.color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			vecVertices.push_back(temp);

			//Bottom right
			temp.pos = XMFLOAT3(rightX, 0.0f, bottomZ);
			temp.color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			vecVertices.push_back(temp);



			//Bottom left
			temp.pos = XMFLOAT3(leftX, 0.0f, bottomZ);
			temp.color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			vecVertices.push_back(temp);



			//Triangle (2)
			//Upper left
			temp.pos = XMFLOAT3(leftX, 0.0f, upperZ);
			temp.color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			vecVertices.push_back(temp);



			//Upper right
			temp.pos = XMFLOAT3(rightX, 0.0f, upperZ);
			temp.color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			vecVertices.push_back(temp);



			//Bottom right
			temp.pos = XMFLOAT3(rightX, 0.0f, bottomZ);
			temp.color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			vecVertices.push_back(temp);


		}
	}

	/*for (j = 0; j < terrainHeight; j++)
		for (i = 0; i < terrainWidth; i++)
		{
			index1 = (terrainHeight*j) + i;					//Bottom left
			index2 = (terrainHeight*j) + (i + 1);			//Bottom right
			index3 = (terrainHeight * (j + 1)) + i;			//Upper left
			index4 = (terrainHeight * (j + 1)) + (i + 1);	//Upper right
			
			//Two vertices per line, three lines per triangle, and two triangles per quad/square. (Borde man inte kunna dra ner det till en vertices per line?)

			//Upper left
			vertices[index].pos = XMFLOAT3(heightMap[index3].x, heightMap[index3].y, heightMap[index3].z);
			vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			indices[index] = index;
			index++;	//index == 0

			//Upper right
			vertices[index].pos = XMFLOAT3(heightMap[index4].x, heightMap[index4].y, heightMap[index4].z);
			vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			indices[index] = index;	
			index++;	//index == 1

			//Upper right
			vertices[index].pos = XMFLOAT3(heightMap[index4].x, heightMap[index4].y, heightMap[index4].z);
			vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			indices[index] = index;
			index++;	//index == 2

			//Bottom left
			vertices[index].pos = XMFLOAT3(heightMap[index1].x, heightMap[index1].y, heightMap[index1].z);
			vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			indices[index] = index;
			index++;	//index == 3

			//Bottom left
			vertices[index].pos = XMFLOAT3(heightMap[index1].x, heightMap[index1].y, heightMap[index1].z);
			vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			indices[index] = index;
			index++;	//index == 4

			//Upper left
			vertices[index].pos = XMFLOAT3(heightMap[index3].x, heightMap[index3].y, heightMap[index3].z);
			vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			indices[index] = index;
			index++;	//index == 5

			//Bottom left
			vertices[index].pos = XMFLOAT3(heightMap[index1].x, heightMap[index1].y, heightMap[index1].z);
			vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			indices[index] = index;
			index++;	//index == 6

			//Upper right
			vertices[index].pos = XMFLOAT3(heightMap[index4].x, heightMap[index4].y, heightMap[index4].z);
			vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			indices[index] = index;
			index++;	//index == 7

			//Upper right
			vertices[index].pos = XMFLOAT3(heightMap[index4].x, heightMap[index4].y, heightMap[index4].z);
			vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			indices[index] = index;
			index++;	//index == 8

			//Bottom right
			vertices[index].pos = XMFLOAT3(heightMap[index2].x, heightMap[index2].y, heightMap[index2].z);
			vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			indices[index] = index;
			index++;	//index == 9

			//Bottom right
			vertices[index].pos = XMFLOAT3(heightMap[index2].x, heightMap[index2].y, heightMap[index2].z);
			vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			indices[index] = index;
			index++;	//index == 10

			//Bottom left
			vertices[index].pos = XMFLOAT3(heightMap[index1].x, heightMap[index1].y, heightMap[index1].z);
			vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			indices[index] = index;
			index++;	//index == 11
		}*/

	D3D11_BUFFER_DESC vertexBufferDesc;
	memset(&vertexBufferDesc, 0, sizeof(vertexBufferDesc));
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	//vertexBufferDesc.ByteWidth = sizeof(VertexData)*sizeof(vertices);
	vertexBufferDesc.ByteWidth = sizeof(VertexData)*vecVertices.size();
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	//vertexData.pSysMem = vertices;
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
	indexData.pSysMem = /*indices*/vecVertices.data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	gDevice->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);

	//delete[] vertices;
	//vertices = 0;

	delete[] indices;
	indices = 0;

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
