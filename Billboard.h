#pragma once
#ifndef BILLBOARD_H
#define BILLBOARD_H

#include "mesh.h"

class Billboard : public Mesh
{
public:
	Billboard();
	~Billboard();
private:
	struct FontType
	{
		float left, right;
		int size;
	};
	ID3D11Texture2D* texture;
	float width, height;
	vector<VertexData> makeQuad(XMFLOAT3 position, XMFLOAT3 camPos);
	void makeTexture(const string & text, ID3D11Device* gDevice);
};

#endif
