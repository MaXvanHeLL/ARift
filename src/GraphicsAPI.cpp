#include "../include/GraphicsAPI.h"
#include "../include/ARiftControl.h"

GraphicsAPI::GraphicsAPI()
{
	m_hinstance_ = GetModuleHandle(NULL);
	m_applicationName_ = "Oculus Rift AR";
}

GraphicsAPI::~GraphicsAPI()
{}

void GraphicsAPI::InitD3D()
{
	// create a struct to hold information about the swap chain
	DXGI_SWAP_CHAIN_DESC scd;
	// clear out the struct for use
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));
	// fill the swap chain description struct
	scd.BufferCount = 1;                                    // one back buffer
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
	scd.OutputWindow = window_;                                // the window to be used
	scd.SampleDesc.Count = 4;                               // how many multisamples
	scd.Windowed = TRUE;                                    // windowed/full-screen mode

	// create a device, device context and swap chain using the information in the scd struct
	D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		NULL,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&scd,
		&swapchain,
		&dev,
		NULL,
		&devcon);

	/*
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = RIFT_RESOLUTION_WIDTH / 2;
	desc.Height = RIFT_RESOLUTION_HEIGHT / 2;
	desc.MipLevels = desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;

	int pixelSize = sizeof(int);//pixel size. Each pixels are represented by a int 32bits.
	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = arift_c->left_undistorted.data; //pixel buffer
	data.SysMemPitch = pixelSize * RIFT_RESOLUTION_WIDTH / 2;// line size in byte
	data.SysMemSlicePitch = pixelSize * RIFT_RESOLUTION_WIDTH / 2 * RIFT_RESOLUTION_HEIGHT / 2;// total buffer size in byte
	*/
	// dx11->pTexture = dx11->dev->CreateTexture2D(&desc, );
}

void GraphicsAPI::render(ARiftControl* arift_c)
{}

void GraphicsAPI::CleanD3D()
{
	// close and release all existing COM objects
	swapchain->Release();
	dev->Release();
	devcon->Release();

	// Remove the window.
	DestroyWindow(window_);
	window_ = NULL;

	// Remove the application instance.
	UnregisterClass(m_applicationName_, m_hinstance_);
	m_hinstance_ = NULL;

}