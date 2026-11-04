#pragma once
class Camera final
{
public:
	Camera(float fov = 90.0f, float aspectRatio = 0.0f, float nearZ = 0.0001f, float farZ = 1000.0f);
	~Camera();
	void SetPosition(XMFLOAT3 newPos);
	void SetRotation(XMFLOAT3 newRot);

	XMFLOAT3 GetPosition() const { return _position; };
	XMFLOAT3 GetRotation() const { return _rotation; };
	XMMATRIX GetViewMat() const { return _viewMat; };
	XMMATRIX GetProjMat() const { return _projMat; };

private:
	void Update();
	void UpdateViewMat();
	void UpdateProjMat(float fov, float aspectRatio, float nearZ, float farZ);

public:
	float fov;
	float aspectRatio;
	float nearZ;
	float farZ;

private:
	XMFLOAT3 _position = XMFLOAT3(0, 0, -5.0f);
	XMFLOAT3 _rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMMATRIX _projMat;
	XMMATRIX _viewMat;
	const XMVECTOR _DEFAULT_FORWARD_VECTOR = XMVectorSet(0, 0, 1, 0);
	const XMVECTOR _DEFAULT_UP_VECTOR = XMVectorSet(0, 1, 0, 0);

};

