#pragma once
#ifndef MESH_H
#define MESH_H

#include "includes.h"
#include <vector>

using namespace std;

class mesh
{
private:
	struct vertex
	{
		XMFLOAT2 UV;
		XMFLOAT3 Normal;
		XMFLOAT3 position;
	};
	vector<vertex> vertices;
	XMFLOAT3 worldPosition;

public:
};

#endif
