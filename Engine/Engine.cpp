#include "Engine.h"

Engine::Engine(HWND handle, UINT width = 0, UINT height = 0)
	: _handle(handle), _width(width), _height(height)
{
}

Engine::~Engine()
{
}

bool Engine::InitEngine()
{
	if (_width <= 0 || _height <= 0) return false;
	Graphics::GetInstance()->Init(_handle, _width, _height);
	_lightBuffer = new ConstantBuffer<DirectionalLight>();
	_lightBuffer->CreateConstantBuffer(ERenderType::RT_PIXEL, 1);

	std::vector<VertexDefault> vertices;
	std::vector<UINT> indices;
	BaseShapes::CreateBox(vertices, indices, 0.1f, XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));
	BaseObject* obj1 = new BaseObject();
	obj1->AddNewObject(vertices, indices);
	baseObjs.push_back(obj1);
	vertices.clear();
	indices.clear();
	BaseShapes::CreateSphere(vertices, indices, 1.0f, 20.0f);
	BaseObject* obj2 = new BaseObject();
	obj2->AddNewObject(vertices, indices);
	baseObjs.push_back(obj2);
	_camera = new Camera(90.0f, static_cast<float>(_width) / _height);
	return true;
}

void Engine::Update()
{
	
	_light.direction = { 0.0f, -1.0f, -1.0f };
	_light.color = { 1.0f, 1.0f, 1.0f, 1.0f };
	_light.ambientColor = { 0.1f, 0.1f, 0.1f };
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
	for (UINT idx = 0 ; idx < baseObjs.size(); ++idx)
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
