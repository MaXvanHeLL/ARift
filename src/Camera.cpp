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
{
  if (old_state_)
    delete old_state_;
}


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
void Camera::SetLookAt(float lookAt)
{
  lookAt_ = lookAt;
}

XMFLOAT3 Camera::GetPosition()
{
	return XMFLOAT3(positionX_, positionY_, positionZ_);
}


XMFLOAT3 Camera::GetRotation()
{
	return XMFLOAT3(rotationX_, rotationY_, rotationZ_);
}
bool Camera::SaveState()
{
  bool overwritten = true;
  if (old_state_ == 0)
  {
    old_state_ = new Camera::State;
    overwritten = false;
  }
  old_state_->lookAt_ = lookAt_;
  old_state_->positionX_ = positionX_;
  old_state_->positionY_ = positionY_;
  old_state_->positionZ_ = positionZ_;
  old_state_->rotationX_ = rotationX_;
  old_state_->rotationY_ = rotationY_;
  old_state_->rotationZ_ = rotationZ_;
  return overwritten;
}

bool Camera::RestoreState()
{
  if (!old_state_)
    return false;

  lookAt_ = old_state_->lookAt_;
  positionX_ = old_state_->positionX_;
  positionY_ = old_state_->positionY_;
  positionZ_ = old_state_->positionZ_;
  rotationX_ = old_state_->rotationX_;
  rotationY_ = old_state_->rotationY_;
  rotationZ_ = old_state_->rotationZ_;
  return true;
}
void Camera::Render()
{
	// XMFLOAT3 up, position, lookAt;
	//float yaw, pitch, roll;
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
	XMVECTOR position = XMVectorSet(positionX_, positionY_, positionZ_, 1.0f);
	/*
	// Setup where the camera is looking by default.
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;
	*/
	XMVECTOR lookAt = XMVectorSet(0.0f, 0.0f, lookAt_, 1.0f);

    //// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
    //pitch = rotationX_ * 0.0174532925f; // 0.0174532925f == 2*pi/360
    //yaw = rotationY_ * 0.0174532925f;
    //roll = rotationZ_ * 0.0174532925f;

    //// Create the rotation matrix from the yaw, pitch, and roll values.
    //rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
  rotationMatrix = XMMatrixRotationRollPitchYaw(rotationX_, rotationY_, rotationZ_);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	// XMStoreFloat3(&lookAt, XMVector3TransformCoord(XMLoadFloat3(&lookAt), rotationMatrix));
	// XMStoreFloat3(&up, XMVector3TransformCoord(XMLoadFloat3(&up), rotationMatrix));
	lookAt = XMVector3TransformCoord(lookAt, rotationMatrix);
	up = XMVector3TransformCoord(up, rotationMatrix);

	// Translate the rotated camera position to the location of the viewer.
	lookAt = position + lookAt;

	// Finally create the view matrix from the three updated vectors.
	// D3DXMatrixLookAtLH(&m_viewMatrix, &position, &lookAt, &up);
	// viewmatrix_ = XMMatrixLookAtLH(position, lookAt, up);
	XMMATRIX viewMatrix_XmMat = XMMatrixLookAtLH(position, lookAt, up);
	XMStoreFloat4x4(&viewmatrix_, viewMatrix_XmMat);

	return;
}

void Camera::TranslateAndRender()
{

	XMMATRIX rotationMatrix;

	XMVECTOR up = XMVectorSet(0.0, -1.0, 0.0, 1.0);

//  XMVECTOR position = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
  XMVECTOR position = XMVectorSet(positionX_,positionY_,positionZ_, 1.0f);
	XMVECTOR lookAt = XMVectorSet(0.0f, 0.0f, lookAt_, 1.0f);

	// Create the rotation matrix from the yaw, pitch, and roll values.
	rotationMatrix = XMMatrixRotationRollPitchYaw(rotationX_, rotationY_, rotationZ_);
  // Create the translation matrix for left or right eye translation from "head" center
//  XMMATRIX translationMatrix = XMMatrixTranslation(positionX_, positionY_, positionZ_);
  // TODO: rewrite comment currently wrong: Combine matrices first translate from center to left/or right eye, then rotate head
//  XMMATRIX transfromMatrix = XMMatrixMultiply(translationMatrix, rotationMatrix);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
//  lookAt = XMVector3TransformCoord(lookAt, transfromMatrix);
//  up = XMVector3TransformCoord(up, transfromMatrix);
//  position = XMVector3TransformCoord(position, transfromMatrix);
  lookAt = XMVector3TransformCoord(lookAt, rotationMatrix);
  up = XMVector3TransformCoord(up, rotationMatrix);
	// Translate the rotated camera position to the location of the viewer.
	lookAt = position + lookAt;

	// Finally create the view matrix from the three updated vectors.
	XMMATRIX viewMatrix_XmMat = XMMatrixLookAtLH(position, lookAt, up);
	XMStoreFloat4x4(&viewmatrix_, viewMatrix_XmMat);
}

void Camera::GetViewMatrix(XMFLOAT4X4& viewMatrix)
{
	viewMatrix = viewmatrix_;
	return;
}