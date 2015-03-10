#include "../include/Texture.h"
#include <iostream>

Texture::Texture()
{
	texture_ = 0;
}


Texture::Texture(const Texture& other)
{}


Texture::~Texture()
{}


bool Texture::Initialize(ID3D11Device* device, WCHAR* filename)
{
	HRESULT result;

	// Load the texture in.
	// result = D3DX11CreateShaderResourceViewFromFile(device, filename, NULL, NULL, &m_texture, NULL);
	result = CreateDDSTextureFromFile(device, filename, nullptr, &texture_);

	if (FAILED(result))
	{
		std::wcout << filename << std::endl;
		return false;
	}

	return true;
}

void Texture::Shutdown()
{
	// Release the texture resource.
	if (texture_)
	{
		texture_->Release();
		texture_ = 0;
	}

	return;
}


ID3D11ShaderResourceView* Texture::GetTexture()
{
	return texture_;
}
