#pragma once
class BaseObject
{
public:
	BaseObject();
	~BaseObject();
	void AddNewObject(std::vector<VertexDefault> vertices, std::vector<UINT> indices);
	void GetViewProj(const Camera* camera);
	virtual void Update();
	void Render();
	void SetPosition(XMFLOAT3 newPos);
	void SetRotation(XMFLOAT3 newRot);
	void SetScale(XMFLOAT3 newScale);
	XMFLOAT3 GetPosition() const { return _location; };
	XMFLOAT3 GetRotation() const { return _rotation; };
	XMFLOAT3 GetScale() const { return _scale; }
private:
	void UpdateTransform();

protected:
	std::unique_ptr<VertexShader<VertexDefault>> vertexShader;
	std::unique_ptr<PixelShader> pixelShader;
	TransformData transformData;
	std::unique_ptr<ConstantBuffer<TransformData>> transformBuffer;
	std::vector<std::unique_ptr<BaseObject>> subObjects;

private:
	XMFLOAT3 _location = XMFLOAT3(0, 0, 0);
	XMFLOAT3 _rotation = XMFLOAT3(0, 0, 0);
	XMFLOAT3 _scale = XMFLOAT3(1, 1, 1);
};

