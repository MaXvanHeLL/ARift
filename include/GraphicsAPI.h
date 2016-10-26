
#ifndef GraphicsAPI_H
#define GraphicsAPI_H
#include <d3d11.h>
#pragma comment (lib, "d3d11.lib")
#include <windows.h>
#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <utility>
#include <vector>
#include "../include/Camera.h"
#include "../include/HeadCamera.h"
#include "../include/Model.h"
#include "../include/Shader.h"
#include "../include/EyeWindow.h"
#include "../include/RenderTexture.h"
#include "../include/OculusHMD.h"
#include "../include/Lighting.h"
#include "../include/LsdSlam3D.h"


#define AR_HMD_ENABLED 1
#define HMD_DISTORTION 1
#define OpenLabNight_DEMO 0

class BitMap;
class ARiftControl;
class Texture;
// -------------------------------

class GraphicsAPI
{
private:
	lsd_slam::LsdSlam3D::RiftPosition3D lsdslam_reference_;
	bool lsdslam_init_ = true;
	lsd_slam::Output3DWrapper* lsdslam3D_;
	ARiftControl* ariftcontrol_;
	ID3D11Device* device_;
	ID3D11DeviceContext* devicecontext_;
	DirectX::XMFLOAT4X4 projectionmatrix_;
	DirectX::XMFLOAT4X4 stereoprojectionmatrix_;
	DirectX::XMFLOAT4X4 worldmatrix_;
	DirectX::XMFLOAT4X4 orthomatrix_;
	int videocardmemory_;
	char videocarddescription_[128];

	float screenDepth_;
	float screenNear_;
	float fieldOfView_;
	float screenAspect_;
	bool undistortionReady_ = false;

	Camera* camera3D_;
  Camera* camera2D_;
  HeadCamera* headCamera_;
  std::vector<Model*> models_;
  int current_model_idx_ = 0;
  Texture* highlight_texture_ = NULL;

  float world_offset_x_ = 0.0f;
  float world_offset_y_ = 0.0f;
  float world_offset_z_ = 0.0f;

	BitMap* bitmap_;
	Shader* shader_;
	Lighting* illumination_;

	ID3D11DepthStencilState* depthDisabledStencilState_;

	// used for Eye Rendering
	void OculusMotion();
	bool RenderToTexture(RenderTexture*,int);
	bool RenderScene(int cam_id);
	bool RenderEyeWindow(EyeWindow*, RenderTexture*);

	// Lsd-Slam
	void UndistortionForLsdSlam(int camID);
	void updateLsdSlam3DPosition();
	bool acquireLsdSlam3DPositionMutex();
	lsd_slam::LsdSlam3D::RiftPosition3D* getLsdSlam3DPosition();
	bool releaseLsdSlam3DPositionMutex();
	void updateLsdSlam3DRotation();
	bool acquireLsdSlam3DRotationMutex();
	XMFLOAT3X3 getLsdSlam3DRotation();
	bool releaseLsdSlam3DRotationMutex();
         
public:
  HANDLE modelsMutex_;
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

	void reInitLsdSlam();

  int SetNextModelActive();
  int SetPreviousModelActive();
  Model::State GetCurrentModelState();
  void SetCurrentModelState(Model::State newState);

	// used for 2D (Bitmaps) - 3D (Models) Rendering on Screen
	void TurnZBufferOn();
	void TurnZBufferOff();

	// used for Oculus Eye Rendering
	ID3D11DepthStencilView* GetDepthStencilView();
	void SetBackBufferRenderTarget();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();
	void GetProjectionMatrix(DirectX::XMFLOAT4X4&);
	void GetStereoProjectionMatrix(DirectX::XMFLOAT4X4&);
	void GetWorldMatrix(DirectX::XMFLOAT4X4&);
	void GetOrthoMatrix(DirectX::XMFLOAT4X4&);
	void GetVideoCardInfo(char*, int&);
	void StereoProjectionTransformation(int camID);
	bool isUndistortionReady();

	// Lsd-Slam 
	void setLsdSlamTrackingAndMapping(lsd_slam::Output3DWrapper* lsdslam3D);

	// Windows stuff
	HINSTANCE hinstance_;
	HWND window_;
	WNDCLASSEX window_class_;
	LPCWSTR applicationName_;
	unsigned int screenWidth_;
	unsigned int screenHeight_;

	// Direct X stuff
	bool vsync_enabled_;
	IDXGISwapChain* swapchain_;
	ID3D11RenderTargetView* rendertargetview_;
	ID3D11Texture2D* depthstencilbuffer_;
	ID3D11DepthStencilState* depthstencilstate_;
	ID3D11DepthStencilView* depthstencilview_;
	ID3D11RasterizerState* rasterstate_;

	// used for Eye Rendering
	HANDLE undistortedShaderMutex_;
	unsigned char* undistortedShaderBuffer_;
	// ID3D11Texture2D* lsdslamUndistortImage_;
	RenderTexture* renderTextureLeft_;
	EyeWindow* eyeWindowLeft_;
	RenderTexture* renderTextureRight_;
	EyeWindow* eyeWindowRight_;

};

#endif // GraphicsAPI_H

