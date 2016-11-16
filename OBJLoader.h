#pragma once
#ifndef OBJLOADER_H
#define OBJLOADER_H

#include "mesh.h"
#include <sstream>
#include <fstream>




class Object : public Mesh
{
private:
struct CONSTANT_BUFFER2
{
	DirectX::XMFLOAT4 KD;
	DirectX::XMFLOAT4 KA;
	DirectX::XMFLOAT4 KS;
};

public:
	/*ID3D11VertexShader* vertexShader = nullptr;
	ID3D11GeometryShader* geometryShader = nullptr;
	ID3D11PixelShader* pixelShader = nullptr;
	ID3D11InputLayout* vertexLayout = nullptr;*/

	Object();
	~Object();

	void LoadObject(ID3D11Device* gDevice);
	void materialCB(ID3D11Device* gDevice);
	void InitializeObjectShaders(ID3D11Device * gDevice);
	void NormalTexture(string normal, ID3D11Device* gDevice);

	CONSTANT_BUFFER2 materialData;
	ID3D11Buffer* VertexBuffer = nullptr;
	ID3D11Buffer* gMaterialBuffer = nullptr;
	ID3D11ShaderResourceView* norTexView;
private:
   void MTLLoader(std::string mtlfile, ID3D11Device* gDevice);
};

#endif