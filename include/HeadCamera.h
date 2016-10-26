#pragma once
#include "Camera.h"
#include "LsdSlam3D.h"

class HeadCamera :
  public Camera
{

public:
  HeadCamera(float positionX, float positionY, float positionZ, float rotationX, float rotationY, float rotationZ);
  virtual ~HeadCamera();
  void RenderEye(bool isLeftEye);

  Camera::Pose3D headToEyeOffset_;

	XMFLOAT3X3 LsdSlamRotationMatrix_;

};

