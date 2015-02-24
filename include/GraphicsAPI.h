
#ifndef GraphicsAPI_H
#define GraphicsAPI_H
#include <opencv2/core/core.hpp>
#include <d3d11.h>
#pragma comment (lib, "d3d11.lib")

#include <windows.h>
// #include <d3d11_1.h>
// #include <d3dcompiler.h>
// #include <directxmath.h>
// #include <directxcolors.h>

// using namespace DirectX;
// #include "CameraInputHandler.h"
// #include "IDSuEyeInputHandler.h"
// #include "../../oculus/ovr_sdk_win_0.4.4/OculusSDK/LibOVR/Src/OVR_CAPI.h" // add oculus sdk to project libs afterwards!
// #include "OVR_CAPI.h"

/*
using namespace DirectX 
struct SimpleVertex
{
	XMFLOAT3 Pos;
};
*/

class GraphicsAPI
{
private:
         
public:
	GraphicsAPI();
	virtual ~GraphicsAPI();

	//** - Description: used for creating the Singleton Object once
	// @return: void
	
	DWORD WINAPI run(LPVOID lpArg);
	void InitD3D();
	void CleanD3D();

	IDXGISwapChain *swapchain;             // the pointer to the swap chain interface
	ID3D11Device *dev;                     // the pointer to our Direct3D device interface
	ID3D11DeviceContext *devcon;
	HWND window_;
	WNDCLASSEX window_class_;
	ID3D11Texture2D* pTexture;
};

#endif // GraphicsAPI_H

