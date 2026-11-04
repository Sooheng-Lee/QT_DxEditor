#pragma once
#include <Windows.h>
#include "pch.h"
#include "DxEngineAPI.h"

class DX_ENGINE_API Engine final
{
public:
	Engine(HWND handle, UINT width, UINT height);
	~Engine();
	bool InitEngine();
	void Update();
	void Render();

private:
	HWND _handle = nullptr;
	UINT _width = 0;
	UINT _height = 0;
	VertexShader<VertexDefault>* _vertexShader;
	PixelShader* _pixelShader;
	ConstantBuffer<TransformData>* _constantBuffer;
};

