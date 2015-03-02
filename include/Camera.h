#ifndef _Camera_H_
#define _Camera_H_

#include <DirectXMath.h>

using namespace DirectX;

class Camera
{
public:
	Camera();
	Camera(const Camera&);
	~Camera();

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();

	void Render();
	void GetViewMatrix(XMMATRIX&);

private:
	float positionX_, positionY_, positionZ_;
	float rotationX_, rotationY_, rotationZ_;
	XMMATRIX viewmatrix_;
};

#endif