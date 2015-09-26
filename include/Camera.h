#ifndef _Camera_H_
#define _Camera_H_

#include <DirectXMath.h>

using namespace DirectX;

class Camera
{
public:
  struct Pose3D {
    float positionX_, positionY_, positionZ_;
    float rotationX_, rotationY_, rotationZ_;
  };

	Camera();
	Camera(const Camera&);
  Camera(float positionX, float positionY, float positionZ, float rotationX, float rotationY, float rotationZ);
	virtual ~Camera();

  void SetPosition(float positionX, float positionY, float positionZ);
  void SetPositionX(float positionX) { currentPose_.positionX_ = positionX; }
  void SetPositionY(float positionY) { currentPose_.positionY_ = positionY; }
  void SetPositionZ(float positionZ) { currentPose_.positionZ_ = positionZ; }

  void SetRotation(float rotationX, float rotationY, float rotationZ);
  void SetRotationX(float rotationX) { currentPose_.rotationX_ = rotationX; }
  void SetRotationY(float rotationY) { currentPose_.rotationY_ = rotationY; }
  void SetRotationZ(float rotationZ) { currentPose_.rotationZ_ = rotationZ; }

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();

	void Render();
  void RenderEyeCamera();
	void GetViewMatrix(XMFLOAT4X4&);
  Camera::Pose3D SavePose();
  void RestorePose();

protected:
	XMFLOAT4X4 viewmatrix_;
  Camera::Pose3D currentPose_;
  Camera::Pose3D oldPose_;
};

#endif