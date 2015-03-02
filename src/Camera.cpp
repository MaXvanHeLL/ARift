#include "../include/Camera.h"

Camera::Camera()
{
	positionX_ = 0.0f;
	positionY_ = 0.0f;
	positionZ_ = 0.0f;

	rotationX_ = 0.0f;
	rotationY_ = 0.0f;
	rotationZ_ = 0.0f;
}


Camera::Camera(const Camera& other)
{}


Camera::~Camera()
{}


void Camera::SetPosition(float x, float y, float z)
{
	positionX_ = x;
	positionY_ = y;
	positionZ_ = z;
	return;
}


void Camera::SetRotation(float x, float y, float z)
{
	rotationX_ = x;
	rotationY_ = y;
	rotationZ_ = z;
	return;
}

XMFLOAT3 Camera::GetPosition()
{
	return XMFLOAT3(positionX_, positionY_, positionZ_);
}


XMFLOAT3 Camera::GetRotation()
{
	return XMFLOAT3(rotationX_, rotationY_, rotationZ_);
}

void Camera::Render()
{
	// XMFLOAT3 up, position, lookAt;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;

	/*
	// Setup the vector that points upwards.
	up.x = 0.0;
	up.y = 1.0;
	up.z = 0.0;
	*/
	XMVECTOR up = XMVectorSet(0.0, 1.0, 0.0, 1.0);
	/*
	// Setup the position of the camera in the world.
	position.x = positionX_;
	position.y = positionY_;
	position.z = positionZ_;
	*/
	XMVECTOR position = XMVectorSet(positionX_, positionY_, positionZ_, 1.0);
	/*
	// Setup where the camera is looking by default.
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;
	*/
	XMVECTOR lookAt = XMVectorSet(0.0, 0.0, 1.0, 1.0);

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	pitch = rotationX_ * 0.0174532925f;
	yaw = rotationY_ * 0.0174532925f;
	roll = rotationZ_ * 0.0174532925f;

	// Create the rotation matrix from the yaw, pitch, and roll values.
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	// XMStoreFloat3(&lookAt, XMVector3TransformCoord(XMLoadFloat3(&lookAt), rotationMatrix));
	// XMStoreFloat3(&up, XMVector3TransformCoord(XMLoadFloat3(&up), rotationMatrix));
	lookAt = XMVector3TransformCoord(lookAt, rotationMatrix);
	up = XMVector3TransformCoord(up, rotationMatrix);

	// Translate the rotated camera position to the location of the viewer.
	lookAt = position + lookAt;

	// Finally create the view matrix from the three updated vectors.
	// D3DXMatrixLookAtLH(&m_viewMatrix, &position, &lookAt, &up);
	viewmatrix_ = XMMatrixLookAtLH(position, lookAt, up);

	return;
}

void Camera::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = viewmatrix_;
	return;
}