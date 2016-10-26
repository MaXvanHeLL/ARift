#ifndef _LIGHTING_H_
#define _LIGHTING_H_

#include <DirectXMath.h>

using namespace DirectX;
/// <summary>
/// Information about the ambient and diffuse lighting that is calculated in the lighting shader is saved here.
/// <\summary>
class Lighting
{
	public:
		Lighting();
		Lighting(const Lighting&);
		~Lighting();

		void SetAmbientColor(float, float, float, float);
		void SetDiffuseColor(float, float, float, float);
		void SetDirection(float, float, float);

		XMFLOAT4 GetAmbientColor();
		XMFLOAT4 GetDiffuseColor();
		XMFLOAT3 GetDirection();

	private:
		XMFLOAT4 ambientColor_;
		XMFLOAT4 diffuseColor_;
		XMFLOAT3 direction_;
};

#endif
