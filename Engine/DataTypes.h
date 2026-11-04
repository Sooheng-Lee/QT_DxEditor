#pragma once
#include <DirectXMath.h>

using namespace DirectX;

struct TransformData
{
	XMMATRIX world;
	XMMATRIX view;
	XMMATRIX proj;
};