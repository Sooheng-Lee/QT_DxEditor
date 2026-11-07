#pragma once
#include <Windows.h>
#include "pch.h"
#include "DxEngineAPI.h"

enum class PrimitiveType
{
	Cube,
	Sphere
};

enum class TransformComponent { Location, Rotation, Scale };

class DX_ENGINE_API Engine final
{
public:
	Engine(HWND handle, UINT width, UINT height);
	~Engine();
	bool InitEngine();
	// Engine owns the returned object until removal or engine destruction.
	BaseObject* CreatePrimitive(PrimitiveType type);
	bool RemoveObject(BaseObject* object);
	bool SetTransformComponent(BaseObject* object, TransformComponent component, int axis, float value);
	void Update();
	void Render();
	void CameraMove(float x, float y, float z);
	void CameraRotate(float x, float y, float z);
private:
	HWND _handle = nullptr;
	UINT _width = 0;
	UINT _height = 0;
	Camera* _camera = nullptr;
	DirectionalLight _light;
	ConstantBuffer<DirectionalLight>* _lightBuffer = nullptr;
	std::vector<BaseObject*> baseObjs;
};

