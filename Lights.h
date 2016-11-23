#pragma once

#ifndef LIGHTS_H
#define LIGHTS_H

#include "includes.h"

class Lights
{
private:
	struct lightData
	{
		XMFLOAT3 noLights;
		XMFLOAT3 position;
		XMFLOAT3 intensity;
		XMFLOAT3 color;
		XMFLOAT3 direction; //for spotlights, 0,0,0 if not spotlight
		XMMATRIX View;
		XMMATRIX Proj;
	}lights[5];
public:
	Lights();
	~Lights();

	void Init(unsigned int lights, ID3D11Device* gDevice);
	ID3D11Buffer* lightBuffer = nullptr;
};

#endif