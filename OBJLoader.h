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
	Object();
	~Object();

	void LoadObject(ID3D11Device* gDevice);
	void materialCB(ID3D11Device* gDevice);
	void InitializeObjectShaders(ID3D11Device * gDevice);
	void NormalTexture(string normal, ID3D11Device* gDevice);
	void Render(ID3D11DeviceContext* gDeviceContext);

	CONSTANT_BUFFER2 materialData;
	ID3D11Buffer* gMaterialBuffer = nullptr;
	ID3D11ShaderResourceView* norTexView;
private:
   void MTLLoader(std::string mtlfile, ID3D11Device* gDevice);
};

#endif