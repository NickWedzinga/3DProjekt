#pragma once
#ifndef OBJLOADER_H
#define OBJLOADER_H

#include "mesh.h"
#include <sstream>
#include <fstream>
#include <WICTextureLoader.h>




class Object : public Mesh
{
private:
struct CONSTANT_BUFFER2
{
	DirectX::XMFLOAT4 KD;
	DirectX::XMFLOAT4 KA;
	DirectX::XMFLOAT4 KS;
};
//struct VertexData
//{
//	float x, y, z, u, v, x2, y2, z2;
//};
public:
	Object();
	~Object();

	void LoadObject(ID3D11Device* gDevice);
	void materialCB(ID3D11Device* gDevice);

	//std::vector<VertexData> triangleVertices; //behövs globalt så att draw kan sättas dynamiskt	
	CONSTANT_BUFFER2 materialData;
	ID3D11Buffer* VertexBuffer = nullptr;
	ID3D11ShaderResourceView* textureView = nullptr;
	ID3D11Buffer* gMaterialBuffer = nullptr;
private:
   void MTLLoader(std::string mtlfile, ID3D11Device* gDevice);
   void Texture(std::string material, ID3D11Device* gDevice);
};

#endif