#include "../include/Camera.h"
#include <iostream>

Camera::Camera()
{
	currentPose_.positionX_ = 0.0f;
	currentPose_.positionY_ = 0.0f;
	currentPose_.positionZ_ = 0.0f;

	currentPose_.rotationX_ = 0.0f;
	currentPose_.rotationY_ = 0.0f;
  currentPose_.rotationZ_ = 0.0f;
}


Camera::Camera(const Camera& other)
{}

Camera::Camera(float positionX, float positionY, float positionZ, float rotationX, float rotationY, float rotationZ)
{
  currentPose_.positionX_ = positionX;
  currentPose_.positionY_ = positionY;
  currentPose_.positionZ_ = positionZ;

  currentPose_.rotationX_ = rotationX;
  currentPose_.rotationY_ = rotationY;
  currentPose_.rotationZ_ = rotationZ;
}

Camera::~Camera()
{

}


void Camera::SetPosition(float positionX, float positionY, float positionZ)
{
  currentPose_.positionX_ = positionX;
  currentPose_.positionY_ = positionY;
  currentPose_.positionZ_ = positionZ;
}


void Camera::SetRotation(float rotationX, float rotationY, float rotationZ)
{
  currentPose_.rotationX_ = rotationX;
  currentPose_.rotationY_ = rotationY;
  currentPose_.rotationZ_ = rotationZ;
}

XMFLOAT3 Camera::GetPosition()
{
  return XMFLOAT3(currentPose_.positionX_, currentPose_.positionY_, currentPose_.positionZ_);
}


XMFLOAT3 Camera::GetRotation()
{
  return XMFLOAT3(currentPose_.rotationX_, currentPose_.rotationY_, currentPose_.rotationZ_);
}

void Camera::Render()
{
	// XMFLOAT3 up, position, lookAt;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;

	// Setup the vector that points upwards.
	XMVECTOR up = XMVectorSet(0.0, 1.0, 0.0, 1.0);
	// Setup the position of the camera in the world.
  XMVECTOR position = XMVectorSet(currentPose_.positionX_, currentPose_.positionY_, currentPose_.positionZ_, 1.0f);
	// Setup where the camera is looking by default.
	XMVECTOR lookAt = XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	pitch = currentPose_.rotationX_ * 0.0174532925f; // 0.0174532925f == 2*pi/360
  yaw = currentPose_.rotationY_ * 0.0174532925f;
  roll = currentPose_.rotationZ_ * 0.0174532925f;

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

Camera::Pose3D Camera::SavePose()
{
  oldPose_.positionX_ = currentPose_.positionX_;
  oldPose_.positionY_ = currentPose_.positionY_;
  oldPose_.positionZ_ = currentPose_.positionZ_;

  oldPose_.rotationX_ = currentPose_.rotationX_;
  oldPose_.rotationY_ = currentPose_.rotationY_;
  oldPose_.rotationZ_ = currentPose_.rotationZ_;

  return oldPose_;
}

void Camera::RestorePose()
{
  currentPose_.positionX_ = oldPose_.positionX_;
  currentPose_.positionY_ = oldPose_.positionY_;
  currentPose_.positionZ_ = oldPose_.positionZ_;

  currentPose_.rotationX_ = oldPose_.rotationX_;
  currentPose_.rotationY_ = oldPose_.rotationY_;
  currentPose_.rotationZ_ = oldPose_.rotationZ_;
}

void Camera::GetViewMatrix(XMFLOAT4X4& viewMatrix)
{
	viewMatrix = viewmatrix_;
}