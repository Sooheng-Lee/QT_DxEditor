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
	void CameraMove(float x, float y, float z);
	void CameraRotate(float x, float y, float z);
private:
	HWND _handle = nullptr;
	UINT _width = 0;
	UINT _height = 0;
	Camera* _camera;
	DirectionalLight _light;
	ConstantBuffer<DirectionalLight>* _lightBuffer;
	std::vector<BaseObject*> baseObjs;
};

