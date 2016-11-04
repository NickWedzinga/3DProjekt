#pragma once
#ifndef INCLUDES_H
#define INCLUDES_H

#include <DirectXMath.h>
#include <windows.h>
#include <SimpleMath.h>
#include <SimpleMath.inl>
#include <vector>
#include <d3d11.h>
#include <d3dcompiler.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

#define WIDTH 640.0f
#define HEIGHT 480.0f
#define XPOS 800.0f
#define YPOS 600.0f
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
