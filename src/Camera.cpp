#include "../include/Camera.h"
#include <iostream>

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

Camera::Camera(float positionX, float positionY, float positionZ, float rotationX, float rotationY, float rotationZ)
{
  positionX_ = positionX;
  positionY_ = positionY;
  positionZ_ = positionZ;

  rotationX_ = rotationX;
  rotationY_ = rotationY;
  rotationZ_ = rotationZ;
}

Camera::~Camera()
{

}


void Camera::SetPosition(float positionX, float positionY, float positionZ)
{
  positionX_ = positionX;
  positionY_ = positionY;
  positionZ_ = positionZ;
}


void Camera::SetRotation(float rotationX, float rotationY, float rotationZ)
{
  rotationX_ = rotationX;
  rotationY_ = rotationY;
  rotationZ_ = rotationZ;
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

	// Setup the vector that points upwards.
	XMVECTOR up = XMVectorSet(0.0, 1.0, 0.0, 1.0);
	// Setup the position of the camera in the world.
	XMVECTOR position = XMVectorSet(positionX_, positionY_, positionZ_, 1.0f);
	// Setup where the camera is looking by default.
	XMVECTOR lookAt = XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	pitch = rotationX_ * 0.0174532925f; // 0.0174532925f == 2*pi/360
	yaw = rotationY_ * 0.0174532925f;
	roll = rotationZ_ * 0.0174532925f;

	// Create the rotation matrix from the yaw, pitch, and roll values.
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	lookAt = XMVector3TransformCoord(lookAt, rotationMatrix);
	up = XMVector3TransformCoord(up, rotationMatrix);

	// Translate the rotated camera position to the location of the viewer.
	lookAt = position + lookAt;

	// Finally create the view matrix from the three updated vectors.
	XMMATRIX viewMatrix_XmMat = XMMatrixLookAtLH(position, lookAt, up);
	XMStoreFloat4x4(&viewmatrix_, viewMatrix_XmMat);
}

Camera::State Camera::SaveState()
{
  old_state_.positionX_ = positionX_;
  old_state_.positionY_ = positionY_;
  old_state_.positionZ_ = positionZ_;

  old_state_.rotationX_ = rotationX_;
  old_state_.rotationY_ = rotationY_;
  old_state_.rotationZ_ = rotationZ_;

  return old_state_;
}

void Camera::RestoreState()
{
  positionX_ = old_state_.positionX_;
  positionY_ = old_state_.positionY_;
  positionZ_ = old_state_.positionZ_;

  rotationX_ = old_state_.rotationX_;
  rotationY_ = old_state_.rotationY_;
  rotationZ_ = old_state_.rotationZ_;
}

void Camera::GetViewMatrix(XMFLOAT4X4& viewMatrix)
{
	viewMatrix = viewmatrix_;
}