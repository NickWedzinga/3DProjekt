#pragma once
#ifndef INCLUDES_H
#define INCLUDES_H

#include <DirectXMath.h>

#define WIDTH 640.0f
#define HEIGHT 480.0f
#define XPOS 800.0f //middle of screen (currently)
#define YPOS 600.0f //middle of screen
#define FOV 80.0f
#define NEAR 0.1f
#define FAR 100.0f

using namespace DirectX;

struct CONSTANT_BUFFER
{
	XMMATRIX WorldMatrix;
	XMMATRIX ViewMatrix;
	XMMATRIX ProjMatrix;
	XMVECTOR camDirection;
	XMVECTOR camPosition;
};

#endif // INCLUDES_H
