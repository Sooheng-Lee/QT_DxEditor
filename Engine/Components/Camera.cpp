#include "../pch.h"
#include "Camera.h"

Camera::Camera(float fov, float aspectRatio, float nearZ, float farZ)
{
	this->fov = fov;
	this->aspectRatio = aspectRatio;
	this->nearZ = nearZ;
	this->farZ = farZ;
	_viewMat = XMMatrixIdentity();
	_projMat = XMMatrixIdentity();
	Update();
}

Camera::~Camera()
{
}

void Camera::SetPosition(XMFLOAT3 newPos)
{
	_position = newPos;
	Update();
}

void Camera::SetRotation(XMFLOAT3 newRot)
{
	_rotation = newRot;
	Update();
}

void BaseObject::UpdateTransform()
{
}

void Camera::Update()
{
	UpdateViewMat();
	UpdateProjMat(fov, aspectRatio, nearZ, farZ);
}

void Camera::UpdateViewMat()
{
	XMVECTOR eye = XMLoadFloat3(&_position);
	
	XMMATRIX rotMat = XMMatrixRotationRollPitchYaw(XMConvertToRadians(_rotation.x), XMConvertToRadians(_rotation.y), XMConvertToRadians(_rotation.z));
	XMVECTOR forward = XMVector4Transform(_DEFAULT_FORWARD_VECTOR, rotMat);
	XMVECTOR up = XMVector4Transform(_DEFAULT_UP_VECTOR, rotMat);
	XMVECTOR right = XMVector3Cross(forward, up);

	float Tx = -XMVectorGetX(XMVector3Dot(eye, right));
	float Ty = -XMVectorGetX(XMVector3Dot(eye, up));
	float Tz = -XMVectorGetX(XMVector3Dot(eye, forward));
	_viewMat = XMMatrixTranspose(
		XMMATRIX(
			XMVectorGetX(right), XMVectorGetX(up), XMVectorGetX(forward), 0.0f,
			XMVectorGetY(right), XMVectorGetY(up), XMVectorGetY(forward), 0.0f,
			XMVectorGetZ(right), XMVectorGetZ(up), XMVectorGetZ(forward), 0.0f,
			Tx, Ty, Tz, 1.0f
		)
	);
}

void Camera::UpdateProjMat(float fov, float aspectRatio, float nearZ, float farZ)
{
	float heightScale = 1 / tan(XMConvertToRadians(fov * 0.5f));
	float widthScale = heightScale / aspectRatio;
	float depthScale = farZ / (farZ - nearZ);
	float depthOffset = -nearZ * depthScale;
	_projMat = XMMatrixTranspose(XMMATRIX(
		widthScale, 0, 0, 0,
		0, heightScale, 0, 0,
		0, 0, depthScale, 1,
		0, 0, depthOffset, 0
	));

}