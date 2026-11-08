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
	delete _lightBuffer;
	delete _camera;
}

bool Engine::InitEngine()
{
	if (_width <= 0 || _height <= 0) return false;
	Graphics::GetInstance()->Init(_handle, _width, _height);
	_lightBuffer = new ConstantBuffer<DirectionalLight>();
	_lightBuffer->CreateConstantBuffer(ERenderType::RT_PIXEL, 1);
	_camera = new Camera(90.0f, static_cast<float>(_width) / _height);
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
    object->GetViewProj(_camera);
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

void Engine::Update()
{
	
	_light.direction = { 0.0f, -1.0f, 1.0f };
	_light.color = { 1.0f, 1.0f, 1.0f, 1.0f };
	_light.ambientColor = { 0.25f, 0.25f, 0.25f };
	_light.intensity = 1.0f;
	_light.shiness = 32.0f;
	_light.cameraPos = _camera->GetPosition();
	_lightBuffer->Update(_light);
	for (UINT idx = 0; idx < baseObjs.size(); ++idx)
	{
		baseObjs[idx]->GetViewProj(_camera);
		baseObjs[idx]->Update();
	}
}

void Engine::Render()
{
	Graphics::GetInstance()->Render();
	_lightBuffer->Render();
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
}

void Engine::CameraRotate(float x, float y, float z)
{
	XMFLOAT3 rotation = _camera->GetRotation();
	rotation.x += x;
	rotation.y += y;
	_camera->SetRotation(rotation);
}
