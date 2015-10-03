#include "../include/Lightning.h"

Lightning::Lightning()
{}


Lightning::Lightning(const Lightning& other)
{}


Lightning::~Lightning()
{}


void Lightning::SetDiffuseColor(float red, float green, float blue, float alpha)
{
	diffuseColor_ = XMFLOAT4(red, green, blue, alpha);
	return;
}


void Lightning::SetDirection(float x, float y, float z)
{
	direction_ = XMFLOAT3(x, y, z);
	return;
}


XMFLOAT4 Lightning::GetDiffuseColor()
{
	return diffuseColor_;
}


XMFLOAT3 Lightning::GetDirection()
{
	return direction_;
}