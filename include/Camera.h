#ifndef _Camera_H_
#define _Camera_H_

#include <DirectXMath.h>

using namespace DirectX;

class Camera
{
public:
  struct State {
    float positionX_, positionY_, positionZ_;
    float rotationX_, rotationY_, rotationZ_;
  };

	Camera();
	Camera(const Camera&);
  Camera(float positionX, float positionY, float positionZ, float rotationX, float rotationY, float rotationZ);
	~Camera();

  void SetPosition(float positionX, float positionY, float positionZ);
  void SetPositionX(float positionX) { positionX_ = positionX; }
  void SetPositionY(float positionY) { positionY_ = positionY; }
  void SetPositionZ(float positionZ) { positionZ_ = positionZ; }

  void SetRotation(float rotationX, float rotationY, float rotationZ);
  void SetRotationX(float rotationX) { rotationX_ = rotationX; }
  void SetRotationY(float rotationY) { rotationY_ = rotationY; }
  void SetRotationZ(float rotationZ) { rotationZ_ = rotationZ; }

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();

	void Render();
	void GetViewMatrix(XMFLOAT4X4&);
  Camera::State SaveState();
  void RestoreState();
private:
	float positionX_, positionY_, positionZ_;
	float rotationX_, rotationY_, rotationZ_;
	XMFLOAT4X4 viewmatrix_;
  Camera::State old_state_;
};

#endif