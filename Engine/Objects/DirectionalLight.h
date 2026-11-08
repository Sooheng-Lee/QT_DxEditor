#pragma once
class DirectionalLight
{
public:
	DirectionalLight();
	~DirectionalLight();
	void CreateLight();
	void Update();
	void Render();
	void SetCameraPos(const XMFLOAT3& position);
	const DirectionalLightData& GetData() const { return _data; }
	void SetData(const DirectionalLightData& data) { _data = data; }
private:
	DirectionalLightData _data;
	std::unique_ptr<ConstantBuffer<DirectionalLightData>> _buffer = nullptr;
};

