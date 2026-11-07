#include "../pch.h"
#include "BaseObject.h"

BaseObject::BaseObject()
{
	vertexShader = std::make_unique<VertexShader<VertexDefault>>();
	pixelShader = std::make_unique<PixelShader>();
	transformBuffer = std::make_unique<ConstantBuffer<TransformData>>();
}

BaseObject::~BaseObject()
{
}

void BaseObject::AddNewObject(std::vector<VertexDefault> vertices, std::vector<UINT> indices)
{
	transformData.world = XMMatrixTranspose(XMMatrixIdentity());
	vertexShader->CreateVertexBuffer(vertices);
	vertexShader->CreateIndexBuffer(indices);
	transformBuffer->CreateConstantBuffer(ERenderType::RT_VERTEX, 0);
	vertexShader->CreateShader(L"..\\Engine\\HLSL\\DefaultColor.hlsl");
	pixelShader->CreateShader(L"..\\Engine\\HLSL\\DefaultColor.hlsl");
}

void BaseObject::GetViewProj(const Camera* camera)
{
	if (!camera) return;
	transformData.view = camera->GetViewMat();
	transformData.proj = camera->GetProjMat();
}



void BaseObject::Update()
{
	transformBuffer->Update(transformData);
}

void BaseObject::Render()
{
	vertexShader->Render();
	pixelShader->Render();
	transformBuffer->Render();
	vertexShader->Draw();
}

void BaseObject::SetPosition(XMFLOAT3 newPos)
{
	_location = newPos;
	UpdateTransform();
}

void BaseObject::SetRotation(XMFLOAT3 newRot)
{
	_rotation = newRot;
	UpdateTransform();
}

void BaseObject::SetScale(XMFLOAT3 newScale)
{
	_scale = newScale;
	UpdateTransform();
}

void BaseObject::UpdateTransform()
{
	XMMATRIX scale = XMMatrixScaling(_scale.x, _scale.y, _scale.z);
	XMMATRIX rotation = XMMatrixRotationRollPitchYaw(_rotation.x, _rotation.y, _rotation.z);
	XMMATRIX translate = XMMatrixTranslation(_location.x, _location.y, _location.z);
	transformData.world = XMMatrixTranspose(scale * rotation * translate);
}