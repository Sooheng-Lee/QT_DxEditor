#include "Engine.h"
#include <algorithm>
#include <cmath>
#include <limits>

Engine::Engine(HWND handle, UINT width = 0, UINT height = 0)
	: _handle(handle), _width(width), _height(height)
{
}

Engine::~Engine()
{
	for (BaseObject* object : baseObjs)
		delete object;
}

bool Engine::InitEngine()
{
	if (_width <= 0 || _height <= 0) return false;
	Graphics::GetInstance()->Init(_handle, _width, _height);
	_camera = std::make_shared<Camera>(90.0f, static_cast<float>(_width) / _height);
    _opaqueState = std::make_unique<BlendState>();
    _opaqueState->CreateBlendState(BlendType::BT_OPAQUE);
    _alphaState = std::make_unique<BlendState>();
    _alphaState->CreateBlendState(BlendType::BT_ALPHA);
    _solidState = std::make_unique<RasterizerState>();
    _solidState->CreateState(RT_SOLID);
    _wireframeState = std::make_unique<RasterizerState>();
    _wireframeState->CreateState(RT_WIREFRAME);
    // Display vertex colors until a scene light is added.
    _previewLight = std::make_unique<ConstantBuffer<DirectionalLightData>>();
    _previewLight->CreateConstantBuffer(ERenderType::RT_PIXEL, 1);
    DirectionalLightData preview;
    preview.ambientColor = XMFLOAT3(1.0f, 1.0f, 1.0f);
    preview.intensity = 0.0f;
    preview.specularStrength = 0.0f;
    _previewLight->Update(preview);
	return true;
}

BaseObject* Engine::CreatePrimitive(PrimitiveType type)
{
    if (!_camera)
        return nullptr;

    std::vector<VertexDefault> vertices;
    std::vector<UINT> indices;
    switch (type)
    {
    case PrimitiveType::Cube:
        BaseShapes::CreateBox(vertices, indices, 1.0f);
        break;
    case PrimitiveType::Sphere:
        BaseShapes::CreateSphere(vertices, indices, 1.0f, 20);
        break;
    case PrimitiveType::Cylinder:
        BaseShapes::CreateCylinder(vertices, indices, 1.0f, 2.0f, 20);
        break;
    default:
        return nullptr;
    }

    auto object = std::make_unique<BaseObject>();
    object->AddNewObject(vertices, indices);
    object->GetViewProj(_camera.get());
    object->Update();
    baseObjs.push_back(object.get());
    return object.release();
}

bool Engine::RemoveObject(BaseObject* object)
{
    const auto it = std::find(baseObjs.begin(), baseObjs.end(), object);
    if (it == baseObjs.end())
        return false;

    baseObjs.erase(it);
    delete object;
    return true;
}

bool Engine::SetTransformComponent(BaseObject* object, TransformComponent component, int axis, float value)
{
    if (axis < 0 || axis > 2 || !std::isfinite(value) ||
        std::find(baseObjs.begin(), baseObjs.end(), object) == baseObjs.end())
        return false;

    XMFLOAT3 vector;
    switch (component)
    {
    case TransformComponent::Location:
        vector = object->GetPosition(); 
        break;
    case TransformComponent::Rotation: 
        vector = object->GetRotation(); 
        break;
    case TransformComponent::Scale: 
        vector = object->GetScale(); 
        break;
    default: return false;
    }
    if (axis == 0) vector.x = value;
    else if (axis == 1) vector.y = value;
    else vector.z = value;

    switch (component)
    {
    case TransformComponent::Location: 
        object->SetPosition(vector); 
        break;
    case TransformComponent::Rotation: 
        object->SetRotation(vector); 
        break;
    case TransformComponent::Scale: 
        object->SetScale(vector); 
        break;
    }
    return true;
}

bool Engine::CreateDirectionalLight()
{
    if (!_camera || _directionalLight)
        return false;

    auto light = std::make_unique<DirectionalLight>();
    light->CreateLight();
    light->SetCameraPos(_camera->GetPosition());
    light->Update();
    _directionalLight = std::move(light);
    return true;
}

bool Engine::HasDirectionalLight() const
{
    return _directionalLight != nullptr;
}

void Engine::RemoveDirectionalLight()
{
    if (!_directionalLight)
        return;

    // D3D retains bound buffers; clear the light slot before releasing the light.
    ID3D11Buffer* emptyBuffer = nullptr;
    Graphics::GetInstance()->GetContext()->PSSetConstantBuffers(1, 1, &emptyBuffer);
    _directionalLight.reset();
}

bool Engine::GetDirectionalLightData(DirectionalLightData& data) const
{
    if (!_directionalLight) return false;
    data = _directionalLight->GetData();
    return true;
}

bool Engine::SetDirectionalLightComponent(LightProperty property, int axis, float value)
{
    if (!_directionalLight || !std::isfinite(value) || axis < 0) return false;
    auto data = _directionalLight->GetData();
    XMFLOAT3* vector = nullptr;
    switch (property)
    {
    case LightProperty::Direction: vector = &data.direction; break;
    case LightProperty::CameraPosition: vector = &data.cameraPos; break;
    case LightProperty::AmbientColor: vector = &data.ambientColor; break;
    case LightProperty::Color:
        if (axis > 3 || value < 0.0f || value > 1.0f) return false;
        if (axis == 0) data.color.x = value;
        else if (axis == 1) data.color.y = value;
        else if (axis == 2) data.color.z = value;
        else data.color.w = value;
        break;
    case LightProperty::Shininess:
        if (axis != 0 || value < 1.0f) return false;
        data.shiness = value;
        break;
    case LightProperty::SpecularStrength:
        if (axis != 0 || value < 0.0f) return false;
        data.specularStrength = value;
        break;
    case LightProperty::Intensity:
        if (axis != 0 || value < 0.0f) return false;
        data.intensity = value;
        break;
    default: return false;
    }
    if (vector)
    {
        if (axis > 2) return false;
        if (property == LightProperty::AmbientColor && (value < 0.0f || value > 1.0f)) return false;
        if (axis == 0) vector->x = value;
        else if (axis == 1) vector->y = value;
        else vector->z = value;
        if (property == LightProperty::Direction &&
            vector->x == 0.0f && vector->y == 0.0f && vector->z == 0.0f) return false;
    }
    _directionalLight->SetData(data);
    return true;
}

void Engine::Update()
{
    if (_directionalLight)
    {
        _directionalLight->Update();
    }
	for (UINT idx = 0; idx < baseObjs.size(); ++idx)
	{
		baseObjs[idx]->GetViewProj(_camera.get());
		baseObjs[idx]->Update();
	}
}

void Engine::Render()
{
	Graphics::GetInstance()->Render();
    if (_directionalLight) {
        _directionalLight->Render();
    }
    else {
        _previewLight->Render();
    }
    _wireframeState->Render();
    _alphaState->Render();

    _opaqueState->Render();
    _solidState->Render();
    for (UINT idx = 0; idx < baseObjs.size(); ++idx)
    {
        baseObjs[idx]->Render();
    }
	Graphics::GetInstance()->SwapBuffers();
}

void Engine::CameraMove(float x, float y, float z)
{
	XMFLOAT3 position = _camera->GetPosition();
	_camera->SetPosition(XMFLOAT3(position.x + x, position.y + y, position.z + z));
    if (_directionalLight)
        _directionalLight->SetCameraPos(_camera->GetPosition());
}

void Engine::CameraRotate(float x, float y, float z)
{
	XMFLOAT3 rotation = _camera->GetRotation();
	rotation.x += x;
	rotation.y += y;
	_camera->SetRotation(rotation);
}

BaseObject* Engine::LoadAsset(const char* filePath)
{
    if (!_camera)
        throw std::runtime_error("The engine is not initialized.");

    const std::vector<MeshData> meshes = ModelLoader::Load(filePath);
    size_t totalVertices = 0;
    size_t totalIndices = 0;
    const size_t maxBufferSize = (std::numeric_limits<UINT>::max)();
    for (const MeshData& mesh : meshes)
    {
        if (mesh.vertices.size() > maxBufferSize / sizeof(VertexDefault) - totalVertices ||
            mesh.indices.size() > maxBufferSize / sizeof(UINT) - totalIndices)
            throw std::runtime_error("The asset exceeds the geometry buffer size limit.");
        totalVertices += mesh.vertices.size();
        totalIndices += mesh.indices.size();
    }
    if (totalVertices == 0 || totalIndices == 0)
        throw std::runtime_error("The asset contains no renderable triangles.");

    const float maxValue = (std::numeric_limits<float>::max)();
    XMFLOAT3 minimum(maxValue, maxValue, maxValue);
    XMFLOAT3 maximum(-maxValue, -maxValue, -maxValue);
    for (const MeshData& mesh : meshes)
    {
        for (const VertexDefault& vertex : mesh.vertices)
        {
            const XMFLOAT3& p = vertex.Position;
            if (!std::isfinite(p.x) || !std::isfinite(p.y) || !std::isfinite(p.z))
                throw std::runtime_error("The asset contains invalid vertex positions.");
            minimum.x = (std::min)(minimum.x, p.x);
            minimum.y = (std::min)(minimum.y, p.y);
            minimum.z = (std::min)(minimum.z, p.z);
            maximum.x = (std::max)(maximum.x, p.x);
            maximum.y = (std::max)(maximum.y, p.y);
            maximum.z = (std::max)(maximum.z, p.z);
        }
        for (UINT index : mesh.indices)
        {
            if (index >= mesh.vertices.size())
                throw std::runtime_error("The asset contains an invalid mesh index.");
        }
    }

    // Frame the asset without changing its original coordinates or scale.
    const XMFLOAT3 center(minimum.x * 0.5f + maximum.x * 0.5f,
        minimum.y * 0.5f + maximum.y * 0.5f, minimum.z * 0.5f + maximum.z * 0.5f);
    const float radius = (std::max)(0.01f, 0.5f * std::hypot(
        maximum.x - minimum.x, maximum.y - minimum.y, maximum.z - minimum.z));
    const float halfFov = XMConvertToRadians(_camera->fov * 0.5f);
    const float halfHorizontalFov = std::atan(std::tan(halfFov) * _camera->aspectRatio);
    const float distance = radius * 1.2f / std::sin((std::min)(halfFov, halfHorizontalFov));
    if (!std::isfinite(distance) || !std::isfinite(distance + radius * 2.0f) ||
        !std::isfinite(center.z - distance))
        throw std::runtime_error("The asset bounds exceed the supported coordinate range.");

    auto object = std::make_unique<BaseObject>();
    object->AddNewObject(meshes);
    baseObjs.push_back(object.get());
    _camera->nearZ = (std::min)(_camera->nearZ, radius * 0.01f);
    _camera->farZ = (std::max)(_camera->farZ, distance + radius * 2.0f);
    _camera->SetRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));
    _camera->SetPosition(XMFLOAT3(center.x, center.y, center.z - distance));
    if (_directionalLight)
        _directionalLight->SetCameraPos(_camera->GetPosition());
    object->GetViewProj(_camera.get());
    object->Update();
    return object.release();
}

void Engine::ResetScene()
{
    for (BaseObject* object : baseObjs)
        delete object;
    baseObjs.clear();
    RemoveDirectionalLight();
    if (_camera)
        _camera = std::make_shared<Camera>(90.0f, static_cast<float>(_width) / _height);
}
