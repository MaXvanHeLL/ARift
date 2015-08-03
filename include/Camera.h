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
    float lookAt_ = 1.0f;
  };

	Camera();
	Camera(const Camera&);
	~Camera();

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();

	void Render();
	void Translate(float, float=0.0f);
	void GetViewMatrix(XMFLOAT4X4&);
  void SetLookAt(float lookAt);
  bool SaveState();
  bool RestoreState();
private:
	float positionX_, positionY_, positionZ_;
	float rotationX_, rotationY_, rotationZ_;
  float lookAt_ = 1.0f;
  Camera::State* old_state_ = 0;
	XMFLOAT4X4 viewmatrix_;
};

#endif