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
enum class LightProperty { Direction, Color, CameraPosition, AmbientColor, Shininess, SpecularStrength, Intensity };

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
	// Returns false when a light already exists or the engine is not initialized.
	bool CreateDirectionalLight();
	bool HasDirectionalLight() const;
	void RemoveDirectionalLight();
	bool GetDirectionalLightData(DirectionalLightData& data) const;
	bool SetDirectionalLightComponent(LightProperty property, int axis, float value);
	void Update();
	void Render();
	void CameraMove(float x, float y, float z);
	void CameraRotate(float x, float y, float z);
private:
	HWND _handle = nullptr;
	UINT _width = 0;
	UINT _height = 0;
	std::unique_ptr<BlendState> _opaqueState;
	std::unique_ptr<BlendState> _alphaState;
	std::unique_ptr<RasterizerState> _solidState;
	std::unique_ptr<RasterizerState> _wireframeState;
	std::shared_ptr<Camera> _camera;
	std::unique_ptr<DirectionalLight> _directionalLight;
	std::vector<BaseObject*> baseObjs;
};

