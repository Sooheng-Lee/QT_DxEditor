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
private:
	void UpdateTransform();

protected:
	std::unique_ptr<VertexShader<VertexDefault>> vertexShader;
	std::unique_ptr<PixelShader> pixelShader;
	TransformData transformData;
	std::unique_ptr<ConstantBuffer<TransformData>> transformBuffer;
	std::vector<std::unique_ptr<BaseObject>> subObjects;
};

