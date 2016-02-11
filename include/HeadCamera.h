#pragma once
#include "Camera.h"
/// <summary>
/// This class is derived from Camera. The calculations needed to get from the head rotation update to the camera parameter update are provided by this class.
/// <\summary>
class HeadCamera :
  public Camera
{

public:
  HeadCamera(float positionX, float positionY, float positionZ, float rotationX, float rotationY, float rotationZ);
  virtual ~HeadCamera();
  void RenderEye(bool isLeftEye);

  Camera::Pose3D headToEyeOffset_;
};

