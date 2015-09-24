#pragma once
#include "Camera.h"
class HeadCamera :
  public Camera
{

public:
  HeadCamera(float positionX, float positionY, float positionZ, float rotationX, float rotationY, float rotationZ);
  virtual ~HeadCamera();
  void RenderEye(bool isLeftEye);

  Camera::Pose headToEyeOffset_;
};

