#include "../pch.h"
#include "DirectionalLight.h"

DirectionalLight::DirectionalLight()
{
	_data.direction = { 0.0f, -1.0f, 1.0f };
	_data.color = { 1.0f, 1.0f, 1.0f, 1.0f };
	_data.ambientColor = { 0.25f, 0.25f, 0.25f };
	_data.intensity = 1.0f;
	_data.shiness = 32.0f;
}

DirectionalLight::~DirectionalLight()
{
}

void DirectionalLight::CreateLight()
{
	_buffer = std::make_unique<ConstantBuffer<DirectionalLightData>>();
	_buffer->CreateConstantBuffer(ERenderType::RT_PIXEL, 1);
}

void DirectionalLight::Update()
{
	_buffer->Update(_data);
}

void DirectionalLight::Render()
{
	_buffer->Render();
}

void DirectionalLight::SetCameraPos(const XMFLOAT3& position)
{
	_data.cameraPos = position;
}
