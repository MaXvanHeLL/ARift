#include "../include/Lighting.h"

Lighting::Lighting()
{}


Lighting::Lighting(const Lighting& other)
{}


Lighting::~Lighting()
{}


void Lighting::SetAmbientColor(float red, float green, float blue, float alpha)
{
	ambientColor_ = XMFLOAT4(red, green, blue, alpha);
	return;
}


void Lighting::SetDiffuseColor(float red, float green, float blue, float alpha)
{
	diffuseColor_ = XMFLOAT4(red, green, blue, alpha);
	return;
}


void Lighting::SetDirection(float x, float y, float z)
{
	direction_ = XMFLOAT3(x, y, z);
	return;
}


XMFLOAT4 Lighting::GetAmbientColor()
{
	return ambientColor_;
}


XMFLOAT4 Lighting::GetDiffuseColor()
{
	return diffuseColor_;
}


XMFLOAT3 Lighting::GetDirection()
{
	return direction_;
}