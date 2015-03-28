#include "../include/Texture.h"
#include "../include/ARiftcontrol.h"
#include <iostream>
#include <fstream>

Texture::Texture()
{
	texture_ = 0;
	cameraTextureLeft_ = 0;

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

bool Texture::InitCameraStream(ID3D11Device* device, ARiftControl* arift_control)
{
	D3D11_TEXTURE2D_DESC tdesc;
	ZeroMemory(&tdesc, sizeof(tdesc));
	D3D11_SUBRESOURCE_DATA srInitData;
	ZeroMemory(&srInitData, sizeof(srInitData));
	ID3D11Texture2D* tex = 0;
	D3D11_SHADER_RESOURCE_VIEW_DESC srDesc;
	ZeroMemory(&srDesc, sizeof(srDesc));

	// NOTE: Just for dirty Synchronisation. If Camera Stream works, doing better solution for this..
	// TODO: implement Camera and Graphics Synchronisation - using Condition Variables | WakeConditionVariable()
	Sleep(500);
	
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
	srInitData.pSysMem = arift_control->cameraBufferLeft_;
	srInitData.SysMemPitch = CAMERA_WIDTH * 4;

	// works 
	if (device->CreateTexture2D(&tdesc, &srInitData, NULL) == S_FALSE)
		std::cout << "Inputs correct" << std::endl;
	else
		std::cout << "wrong inputs" << std::endl;

	if (FAILED(device->CreateTexture2D(&tdesc, &srInitData, &tex)))
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

	HRESULT result = device->CreateShaderResourceView(tex, &srDesc, &texture_);
	std::cout << result << std::endl;

	if (SUCCEEDED(device->CreateShaderResourceView(tex, &srDesc, &texture_)));
	{
		std::cerr << "Can't create Shader Resource View" << std::endl;
		return true;
	}

	return false;
}

bool Texture::Update(ID3D11Device* device, WCHAR* filename)
{
	HRESULT result;


	// ------------------ [2] ----------------------------
	// Load the texture from memory
	/*
	file.seekg(0, std::ios::end);
	int length = file.tellg();
	file.seekg(0, std::ios::beg);

	unsigned char* buffer = new unsigned char[length];
	file.read(&buffer[0], length);
	file.close();

	HRESULT hr;
	hr = DirectX::CreateWICTextureFromMemory(_D3D->GetDevice(), _D3D->GetDeviceContext(), &buffer[0], (size_t)length, nullptr, &srv, NULL);
	*/

	// ------------------------------ [2] ----------------------------------------
	/*
	cv::Mat inputMat = getInputTexture();
	91

	92         D3D10_TEXTURE2D_DESC desc = { 0 };
	93         desc.Width = inputMat.size().width;
	94         desc.Height = inputMat.size().height;
	95         desc.MipLevels = desc.ArraySize = 1;
	96         desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	97         desc.SampleDesc.Count = 1;
	98         desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
	99         desc.Usage = D3D10_USAGE_IMMUTABLE;
	100         desc.CPUAccessFlags = cv::ocl::useOpenCL() ? 0 : D3D10_CPU_ACCESS_READ;
	101

	102         D3D10_SUBRESOURCE_DATA srInitData;
	103         srInitData.pSysMem = inputMat.ptr();
	104         srInitData.SysMemPitch = (UINT)inputMat.step[0];
	105

	106         if (FAILED(dev->CreateTexture2D(&desc, &srInitData, &pInputTexture)))
	*/
	return true;
}

void Texture::Shutdown()
{

	void* buffer = ::CoTaskMemAlloc(600 * 400 * 3);
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
