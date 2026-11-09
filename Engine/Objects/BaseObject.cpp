#include "../pch.h"
#include "BaseObject.h"

BaseObject::BaseObject()
{
	vertexShader = std::make_unique<VertexShader<VertexDefault>>();
	pixelShader = std::make_unique<PixelShader>();
	diffuseTexture = std::make_unique<ShaderResourceView>();
	materialBuffer = std::make_unique<ConstantBuffer<CBMaterial>>();
	transformBuffer = std::make_unique<ConstantBuffer<TransformData>>();
}

BaseObject::~BaseObject()
{
}

void BaseObject::AddNewObject(std::vector<VertexDefault> vertices, std::vector<UINT> indices, const MaterialData& material)
{
	subObjects.clear();
	if (material.diffuseTexPath.empty())
	{
		const uint8_t white[] = { 255, 255, 255, 255 };
		diffuseTexture->CreateShaderResourceView(1, 1, white);
	}
	else
	{
		diffuseTexture->LoadFromFile(material.diffuseTexPath);
	}
	CBMaterial data = {};
	data.DiffuseColor = material.diffuseColor;
	data.SpecularColor = material.specularColor;
	data.Shininess = material.shininess;
	materialBuffer->CreateConstantBuffer(ERenderType::RT_PIXEL, 2);
	materialBuffer->Update(data);
	transformData.world = XMMatrixTranspose(XMMatrixIdentity());
	vertexShader->CreateVertexBuffer(vertices);
	vertexShader->CreateIndexBuffer(indices);
	transformBuffer->CreateConstantBuffer(ERenderType::RT_VERTEX, 0);
	vertexShader->CreateShader(L"HLSL\\DefaultColor.hlsl");
	pixelShader->CreateShader(L"HLSL\\DefaultColor.hlsl");
}

void BaseObject::AddNewObject(const std::vector<MeshData>& meshes)
{
	std::vector<std::unique_ptr<BaseObject>> objects;
	for (const auto& mesh : meshes)
	{
		if (mesh.vertices.empty() || mesh.indices.empty()) continue;
		auto object = std::make_unique<BaseObject>();
		object->AddNewObject(mesh.vertices, mesh.indices, mesh.material);
		objects.push_back(std::move(object));
	}
	if (objects.empty()) throw std::runtime_error("The asset contains no renderable meshes.");
	subObjects = std::move(objects);
	UpdateTransform();
}

void BaseObject::GetViewProj(const Camera* camera)
{
	if (!camera) return;
	transformData.view = camera->GetViewMat();
	transformData.proj = camera->GetProjMat();
}



void BaseObject::Update()
{
	if (!subObjects.empty())
	{
		for (auto& object : subObjects)
		{
			object->transformData = transformData;
			object->Update();
		}
		return;
	}
	transformBuffer->Update(transformData);
}

void BaseObject::Render()
{
	if (!subObjects.empty())
	{
		for (auto& object : subObjects) object->Render();
		return;
	}
	materialBuffer->Render();
	diffuseTexture->Render();
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
	XMMATRIX location = XMMatrixTranslation(_location.x, _location.y, _location.z);
	transformData.world = XMMatrixTranspose(scale * rotation * location);
}
