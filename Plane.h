#pragma once
#ifndef PLANE_H
#define PLANE_H

#include "includes.h"
#include "mesh.h"

class Plane : public Mesh
{
public:
	Plane(int ID);
	~Plane();
	void CreatePlane(XMFLOAT3 pos, int width, int height, int depth, ID3D11Device* gDevice);
	void Render(ID3D11DeviceContext* gDeviceContext);
private:

};

#endif
