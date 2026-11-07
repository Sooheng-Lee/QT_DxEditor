#pragma once
#include <DirectXMath.h>

using namespace DirectX;

struct TransformData
{
	XMMATRIX world;
	XMMATRIX view;
	XMMATRIX proj;
};

struct DirectionalLight
{
	XMFLOAT3 direction = { 0.0f, -1.0f, 1.0f };
	float shiness = 32.0f;
	
	XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f };

	XMFLOAT3 cameraPos = { 0.0f, 0.0f, -3.0f };
	float specularStrength = 0.5f;

	XMFLOAT3 ambientColor = { 0.1f, 0.1f, 0.1f };
	float intensity = 1.0f;
};