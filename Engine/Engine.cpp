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
	std::vector<VertexDefault> vertices;
	std::vector<UINT> indices;
	BaseShapes::CreateBox(vertices, indices, 0.1f, XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));
	_vertexShader->CreateVertexBuffer(vertices);
	_vertexShader->CreateIndexBuffer(indices);
	_vertexShader->CreateShader(L"C:\\Users\\Note\\OneDrive\\Documents\\QT_DxEditor\\Engine\\HLSL\\DefaultColor.hlsl");
	_pixelShader->CreateShader(L"C:\\Users\\Note\\OneDrive\\Documents\\QT_DxEditor\\Engine\\HLSL\\DefaultColor.hlsl");
}

void Engine::Update()
{
}

void Engine::Render()
{
	Graphics::GetInstance()->Render();
	_vertexShader->Render();
	_pixelShader->Render();

	_vertexShader->Draw();
	Graphics::GetInstance()->SwapBuffers();
}
