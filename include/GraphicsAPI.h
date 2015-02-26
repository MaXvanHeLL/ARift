
#ifndef GraphicsAPI_H
#define GraphicsAPI_H
#include <opencv2/core/core.hpp>
#include <d3d11.h>
#pragma comment (lib, "d3d11.lib")
#include <windows.h>
#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <DirectXMath.h>

class ARiftControl;

class GraphicsAPI
{
private:
	ID3D11Device* device_;
	ID3D11DeviceContext* devicecontext_;
	DirectX::XMMATRIX projectionmatrix_;
	DirectX::XMMATRIX worldmatrix_;
	DirectX::XMMATRIX orthomatrix_;
	int videocardmemory_;
	char videocarddescription_[128];
         
public:
	GraphicsAPI();
	virtual ~GraphicsAPI();
	
	DWORD WINAPI run(LPVOID lpArg);
	bool InitD3D(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen,
		float screenDepth, float screenNear);
	void render(ARiftControl* arift_c);
	void shutDownD3D();

	void BeginScene(float, float, float, float);
	void EndScene();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();
	void GetProjectionMatrix(DirectX::XMMATRIX&);
	void GetWorldMatrix(DirectX::XMMATRIX&);
	void GetOrthoMatrix(DirectX::XMMATRIX&);
	void GetVideoCardInfo(char*, int&);

	// Windows stuff
	HINSTANCE hinstance_;
	HWND window_;
	WNDCLASSEX window_class_;
	LPCWSTR applicationName_;
	unsigned int screenwidth_;
	unsigned int screenheight_;

	// Direct X stuff
	bool vsync_enabled_;
	IDXGISwapChain* swapchain_;
	ID3D11RenderTargetView* rendertargetview_;
	ID3D11Texture2D* depthstencilbuffer_;
	ID3D11DepthStencilState* depthstencilstate_;
	ID3D11DepthStencilView* depthstencilview_;
	ID3D11RasterizerState* rasterstate_;
};

#endif // GraphicsAPI_H

