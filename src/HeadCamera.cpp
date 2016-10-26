#include "../include/HeadCamera.h"
#include <iostream>

HeadCamera::HeadCamera(float positionX, float positionY, float positionZ, float rotationX, float rotationY, float rotationZ)
{
  headToEyeOffset_.positionX_ = 0.0f;
  headToEyeOffset_.positionY_ = 0.0f;
  headToEyeOffset_.positionZ_ = 0.0f;

  headToEyeOffset_.rotationX_ = 0.0f;
  headToEyeOffset_.rotationY_ = 0.0f;
  headToEyeOffset_.rotationZ_ = 0.0f;

  currentPose_.positionX_ = positionX;
  currentPose_.positionY_ = positionY;
  currentPose_.positionZ_ = positionZ;

  currentPose_.rotationX_ = rotationX;
  currentPose_.rotationY_ = rotationY;
  currentPose_.rotationZ_ = rotationZ;

  XMFLOAT3X3 LsdSlamRotationMatrix_ = {};
}


HeadCamera::~HeadCamera()
{
}

void HeadCamera::RenderEye(bool isLeftEye)
{
  // Setup the vector that points upwards.
  XMVECTOR up = XMVectorSet(0.0, 1.0, 0.0, 1.0);
  // Setup the position of the camera in the world.
  XMVECTOR position = XMVectorSet(currentPose_.positionX_, currentPose_.positionY_, currentPose_.positionZ_, 1.0f);
  // Setup the position offset from head center to eyecenter.
  XMVECTOR headToEyePositionOffset = XMVectorSet(
    isLeftEye ? (-headToEyeOffset_.positionX_) : headToEyeOffset_.positionX_,
    headToEyeOffset_.positionY_,
    headToEyeOffset_.positionZ_,
    1.0f);

  // Setup where the camera is looking by default.
  XMVECTOR lookAt = XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);

	XMMATRIX rotationMatrix = {};
	if (LsdSlam_Rotation)
	{
		XMFLOAT4X4 graphicsRotation = {};
		rotationMatrix = XMLoadFloat3x3(&LsdSlamRotationMatrix_);
		//XMStoreFloat4x4(&graphicsRotation, rotationMatrix);
		//graphicsRotation._41 = 0.f;
		//graphicsRotation._42 = 0.f;
		//graphicsRotation._43 = 0.f;
		//graphicsRotation._44 = 1.f;
		//graphicsRotation._14 = 0.f;
		//graphicsRotation._24 = 0.f;
		//graphicsRotation._34 = 0.f;

		//rotationMatrix = XMLoadFloat4x4(&graphicsRotation);
	}
	else
	{
		// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
		float pitch = (currentPose_.rotationX_ + headToEyeOffset_.rotationX_) * 0.0174532925f; // 0.0174532925f == 2*pi/360
		float yaw = (currentPose_.rotationY_ + headToEyeOffset_.rotationY_) * 0.0174532925f;
		float roll = (currentPose_.rotationZ_ + headToEyeOffset_.rotationZ_) * 0.0174532925f;

		// Create the rotation matrix from the yaw, pitch, and roll values.
		rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
	}

  // Transform the lookAt and up vector so that eyes rotate correctly with head movement
  lookAt = XMVector3TransformCoord(lookAt + headToEyePositionOffset, rotationMatrix);
  up = XMVector3TransformCoord(up, rotationMatrix);

  // Translate the rotated camera position to the location of the viewer.
  lookAt = position + lookAt;

  // Finally create the view matrix from the three updated vectors.
  XMMATRIX viewMatrix_XmMat = XMMatrixLookAtLH(position + headToEyePositionOffset, lookAt, up);
  XMStoreFloat4x4(&viewmatrix_, viewMatrix_XmMat);
}
