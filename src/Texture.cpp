#include "../include/Texture.h"
#include "../include/ARiftcontrol.h"
#include <iostream>
#include <fstream>

Texture::Texture()
{
	shaderResource_ = 0;
	cameraTexture_ = 0;
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
	result = CreateDDSTextureFromFile(device, filename, nullptr, &shaderResource_);
	if (FAILED(result))
	{
		std::wcout << filename << std::endl;
		return false;
	}

	return true;
}

bool Texture::InitCameraStream(ID3D11Device* device, ARiftControl* arift_control)
{
	D3D11_TEXTURE2D_DESC tdesc;
	ZeroMemory(&tdesc, sizeof(tdesc));
	D3D11_SUBRESOURCE_DATA srInitData;
	ZeroMemory(&srInitData, sizeof(srInitData));
	D3D11_SHADER_RESOURCE_VIEW_DESC srDesc;
	ZeroMemory(&srDesc, sizeof(srDesc));

	// NOTE: Just for dirty Synchronisation. If Camera Stream works, doing better solution for this..
	// Sleep(500);
	
	tdesc.ArraySize = 1;
	tdesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	tdesc.Usage = D3D11_USAGE_DYNAMIC;
	tdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	tdesc.Height = CAMERA_HEIGHT;
	tdesc.Width = CAMERA_WIDTH;
	tdesc.MipLevels = 1;
	tdesc.MiscFlags = 0;
	tdesc.SampleDesc.Count = 1;
	tdesc.SampleDesc.Quality = 0;

	// NOTE: "cameraBufferLeft_" contains RGBA8 Data. ColorMode of Camera set to IS_CM_RGBA8_PACKED
	std::cout << "Render Thread: waiting for mutex" << std::endl;
	WaitForSingleObject(arift_control->cameraMutexLeft_, INFINITE); // lock
	std::cout << "Render Thread: mutex acquired" << std::endl;
	srInitData.pSysMem = arift_control->cameraBufferLeft_;
	ReleaseMutex(arift_control->cameraMutexLeft_); // unlock
	srInitData.SysMemPitch = CAMERA_WIDTH * 4;

	// works 
	if (device->CreateTexture2D(&tdesc, &srInitData, NULL) == S_FALSE)
		std::cout << "Inputs correct" << std::endl;
	else
		std::cout << "wrong inputs" << std::endl;

	if (FAILED(device->CreateTexture2D(&tdesc, &srInitData, &cameraTexture_)))
	{
		std::cout << "Failed" << std::endl;
		return(false);
	}
	else
		std::cout << "Sucess" << std::endl;
		
  //  Create the shader-resource view
	srDesc.Format = tdesc.Format;
	srDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srDesc.Texture2D.MostDetailedMip = 0;
	srDesc.Texture2D.MipLevels = 1;

	if (SUCCEEDED(device->CreateShaderResourceView(cameraTexture_, &srDesc, &shaderResource_)));
	{
		return true;
	}

	std::cerr << "Can't create Shader Resource View" << std::endl;
	return false;
}

bool Texture::Update(ID3D11DeviceContext* devicecontext, ARiftControl* arift_control)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	WaitForSingleObject(arift_control->cameraMutexLeft_, INFINITE);
	unsigned char* cameraBuffer = arift_control->cameraBufferLeft_;
	ReleaseMutex(arift_control->cameraMutexLeft_);

	if (FAILED(devicecontext->Map(cameraTexture_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		std::cout << "Error: could not update Bitmap Texture!" << std::endl;
		return false;
	}
	BYTE* mappedData = reinterpret_cast<BYTE*>(mappedResource.pData);
	for (UINT i = 0; i < CAMERA_HEIGHT; i++)
	{
		memcpy(mappedData, cameraBuffer, CAMERA_WIDTH * 4);
		mappedData += mappedResource.RowPitch;
		cameraBuffer += CAMERA_WIDTH * 4;
	}

	// memcpy(mappedResource.pData, arift_control->cameraBufferLeft_, CAMERA_BUFFER_LENGTH);
	devicecontext->Unmap(cameraTexture_, 0);
	return true;
}

void Texture::Shutdown()
{

	// Release the texture resource.
	if (shaderResource_)
	{
		shaderResource_->Release();
		shaderResource_ = 0;
	}

	return;
}


ID3D11ShaderResourceView* Texture::GetTexture()
{
	return shaderResource_;
}
