
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
#include "../include/Camera.h"
#include "../include/Model.h"
#include "../include/Shader.h"
// #include "../include/BitMap.h"
#include "../include/EyeWindow.h"
#include "../include/RenderTexture.h"
#include "../include/OculusHMD.h"

#define AR_HMD_ENABLED 1
#define HMD_DISTORTION 1
class BitMap;
class ARiftControl;
// -------------------------------

class GraphicsAPI
{
private:
	ARiftControl* ariftcontrol_;
	ID3D11Device* device_;
	ID3D11DeviceContext* devicecontext_;
	DirectX::XMFLOAT4X4 projectionmatrix_;
	DirectX::XMFLOAT4X4 worldmatrix_;
	DirectX::XMFLOAT4X4 orthomatrix_;
	int videocardmemory_;
	char videocarddescription_[128];

	Camera* camera_;
	Model* model_;
	BitMap* bitmap_;
	Shader* shader_;

	ID3D11DepthStencilState* depthDisabledStencilState_;

	// used for Eye Rendering
	bool RenderToTexture(RenderTexture*,int);
	bool RenderScene(int cam_id);
	bool RenderEyeWindow(EyeWindow*, RenderTexture*);
         
public:
	GraphicsAPI();
	virtual ~GraphicsAPI();
	
	DWORD WINAPI run(LPVOID lpArg);
	bool InitD3D(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen,
		           float screenDepth, float screenNear, ARiftControl* arift_control); 
	bool Frame();
	bool Render();
	void shutDownD3D();

	void BeginScene(float, float, float, float);
	void EndScene();

	// used for 2D (Bitmaps) - 3D (Models) Rendering on Screen
	void TurnZBufferOn();
	void TurnZBufferOff();

	// used for Oculus Eye Rendering
	ID3D11DepthStencilView* GetDepthStencilView();
	void SetBackBufferRenderTarget();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();
	void GetProjectionMatrix(DirectX::XMFLOAT4X4&);
	void GetWorldMatrix(DirectX::XMFLOAT4X4&);
	void GetOrthoMatrix(DirectX::XMFLOAT4X4&);
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

	// used for Eye Rendering
	RenderTexture* renderTextureLeft_;
	EyeWindow* eyeWindowLeft_;
	RenderTexture* renderTextureRight_;
	EyeWindow* eyeWindowRight_;
};

#endif // GraphicsAPI_H

