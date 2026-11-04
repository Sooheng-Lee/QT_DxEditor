#include "Engine.h"

Engine::Engine(HWND handle, UINT width = 0, UINT height = 0)
	: _handle(handle), _width(width), _height(height)
{
}

Engine::~Engine()
{
	if (_vertexShader)
	{
		delete _vertexShader;
		_vertexShader = nullptr;
	}

	if (_pixelShader)
	{
		delete _pixelShader;
		_pixelShader = nullptr;
	}
}

bool Engine::InitEngine()
{
	if (_width <= 0 || _height <= 0) return false;
	Graphics::GetInstance()->Init(_handle, _width, _height);
	_vertexShader = new VertexShader<VertexDefault>();
	_pixelShader = new PixelShader();
	_constantBuffer = new ConstantBuffer<TransformData>();
	_constantBuffer->CreateConstantBuffer(ERenderType::RT_VERTEX, 0);

	std::vector<VertexDefault> vertices;
	std::vector<UINT> indices;
	//BaseShapes::CreateBox(vertices, indices, 0.1f, XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));
	BaseShapes::CreateSphere(vertices, indices, 1.0f, 20.0f);
	_vertexShader->CreateVertexBuffer(vertices);
	_vertexShader->CreateIndexBuffer(indices);
	_vertexShader->CreateShader(L"D:\\CPP\\QT_DxEditor\\Engine\\HLSL\\DefaultColor.hlsl");
	_pixelShader->CreateShader(L"D:\\CPP\\QT_DxEditor\\Engine\\HLSL\\DefaultColor.hlsl");
	_camera = new Camera(90.0f, static_cast<float>(_width) / _height);
}

void Engine::Update()
{
	static TransformData data = {};
	static XMFLOAT3 position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	static XMFLOAT3 rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	rotation = XMFLOAT3(rotation.x + 0.0001f, 0.0f, 0.0f);
	_camera->SetRotation(rotation);
	data.world = XMMatrixTranslation(position.x, position.y, position.z);
	data.world = XMMatrixTranspose(data.world);
	data.view = _camera->GetViewMat();
	data.proj = _camera->GetProjMat();
	_constantBuffer->Update(data);
}

void Engine::Render()
{
	Graphics::GetInstance()->Render();
	_vertexShader->Render();
	_pixelShader->Render();
	_constantBuffer->Render();
	_vertexShader->Draw();
	Graphics::GetInstance()->SwapBuffers();
}
