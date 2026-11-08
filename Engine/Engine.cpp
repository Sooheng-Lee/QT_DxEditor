#include "Engine.h"
#include <algorithm>
#include <cmath>

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
    //ID3D11Buffer* emptyBuffer = nullptr;
    //Graphics::GetInstance()->GetContext()->PSSetConstantBuffers(1, 1, &emptyBuffer);
    //_directionalLight.reset();
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
