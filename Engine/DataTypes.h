#pragma once
#include <DirectXMath.h>
#include "VertexTypes.h"

using namespace DirectX;

struct TransformData
{
	XMMATRIX world;
	XMMATRIX view;
	XMMATRIX proj;
};

struct DirectionalLightData
{
	XMFLOAT3 direction = { 0.0f, -1.0f, 1.0f };
	float shiness = 32.0f;
	
	XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f };

	XMFLOAT3 cameraPos = { 0.0f, 0.0f, -3.0f };
	float specularStrength = 0.5f;

	XMFLOAT3 ambientColor = { 0.1f, 0.1f, 0.1f };
	float intensity = 1.0f;
};

struct MeshData
{
	std::vector<VertexDefault> vertices;
	std::vector<uint32_t> indices;

	unsigned int materialIdx = 0;
};