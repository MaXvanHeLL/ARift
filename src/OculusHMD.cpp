#include "../include/OculusHMD.h"
#include "../include/GraphicsAPI.h"
#include "../include/ARiftControl.h"
#include <iostream>
#include <algorithm> 
#define   OVR_D3D_VERSION 11
#include "OVR_CAPI_D3D.h" 

// #define OVR_D3D_VERSION 11
// #include "OVR_CAPI.h"
// #include "OVR_CAPI_D3D.h"
// #include "Kernel/OVR_Math.h"

// #include <d3d11.h>
// #pragma comment (lib, "d3d11.lib")

OculusHMD* OculusHMD::instance_ = NULL;

OculusHMD::OculusHMD()
{
	if (!instance_)
	{
		ovr_Initialize();

		hmd_ = ovrHmd_Create(0); // (0) means: create first available HMD - indexed access
		if (hmd_)
		{
			resolution_ = hmd_->Resolution;

			// setup for sensors and motion tracking
			ovrHmd_ConfigureTracking(hmd_, ovrTrackingCap_Orientation | ovrTrackingCap_MagYawCorrection |
				ovrTrackingCap_Position, 0);
		}
		else
		{
			// do error stuff here
		}
		running_ = true;
	}
}

// -----------------------------------------------------------------------     
OculusHMD::~OculusHMD()
{
	ovrHmd_Destroy(hmd_);
	ovr_Shutdown();
	instance_ = NULL;
}

OculusHMD* OculusHMD::instance()
{
	if (!instance_)
		instance_ = new OculusHMD();

	return instance_;
}

// -----------------------------------------------------------------------     
void OculusHMD::initialization(GraphicsAPI* graphicsAPI)
{
	if (!instance_)
	{
		instance_ = new OculusHMD();
		instance_->graphicsAPI_ = graphicsAPI;
	}
}

// -----------------------------------------------------------------------     
void OculusHMD::trackMotion(float& yaw, float& eyepitch, float& eyeroll)
{
	// get current tracking state
	if (hmd_)
	{
		ovrTrackingState tracking_state = ovrHmd_GetTrackingState(hmd_, ovr_GetTimeInSeconds());

		if (tracking_state.StatusFlags & (ovrStatus_OrientationTracked | ovrStatus_PositionTracked))
		{
			OVR::Posef pose = tracking_state.HeadPose.ThePose;
			pose.Rotation.GetEulerAngles<Axis_Y, Axis_X, Axis_Z>(&yaw, &eyepitch, &eyeroll);

			// for debug purposes only
			cout << "yaw: " << RadToDegree(yaw) << endl;
			cout << "pitch: " << RadToDegree(eyepitch) << endl;
			cout << "roll: " << RadToDegree(eyeroll) << endl << endl;
		}
	}
}


// -----------------------------------------------------------------------     
void OculusHMD::calculateFOV()
{
	if (hmd_)
	{
		for (int eye = 0; eye<2; eye++)
		{
			eyeSize_[eye] = ovrHmd_GetFovTextureSize(hmd_, (ovrEyeType)eye,
			hmd_->DefaultEyeFov[eye], 1.0f);
		}
	}
}


// -----------------------------------------------------------------------     
void OculusHMD::configureStereoRendering()
{
	ovrD3D11Config d3d11cfg;
	d3d11cfg.D3D11.Header.API = ovrRenderAPI_D3D11;
	d3d11cfg.D3D11.Header.BackBufferSize = Sizei(hmd_->Resolution.w, hmd_->Resolution.h);
	d3d11cfg.D3D11.Header.Multisample = 1;
	d3d11cfg.D3D11.pDevice = graphicsAPI_->GetDevice();
	d3d11cfg.D3D11.pDeviceContext = graphicsAPI_->GetDeviceContext();
	d3d11cfg.D3D11.pBackBufferRT = graphicsAPI_->rendertargetview_;
	d3d11cfg.D3D11.pSwapChain = graphicsAPI_->swapchain_;

	if (!ovrHmd_ConfigureRendering(hmd_, &d3d11cfg.Config,
		ovrDistortionCap_Chromatic | ovrDistortionCap_Vignette |
		ovrDistortionCap_TimeWarp | ovrDistortionCap_Overdrive,
		hmd_->DefaultEyeFov, eyeRenderDesc_))
	{
		std::cout << "HMD Error: could not ConfigureRendering!" << std::endl;
	}

	useHmdToEyeViewOffset_[0] = eyeRenderDesc_[0].HmdToEyeViewOffset;
	useHmdToEyeViewOffset_[1] = eyeRenderDesc_[1].HmdToEyeViewOffset;

	ovrHmd_GetEyePoses(hmd_, 0, useHmdToEyeViewOffset_, eyeRenderPose_, NULL);

	ovrHmd_AttachToWindow(OculusHMD::instance()->hmd_, graphicsAPI_->window_, NULL, NULL);

	// disable health and security warnings 
	ovrHmd_DismissHSWDisplay(hmd_);
}

void OculusHMD::StartFrames()
{
	ovrHmd_BeginFrame(hmd_, 0);
}


bool OculusHMD::RenderDistortion()
{
	ovrD3D11Texture eyeTexture[2]; // Gather data for eye textures 
	Sizei size;
	size.w = RIFT_RESOLUTION_WIDTH; 
	size.h = RIFT_RESOLUTION_HEIGHT;

	ovrRecti eyeRenderViewport[2];
	eyeRenderViewport[0].Pos = Vector2i(0, 0);
	eyeRenderViewport[0].Size = size;
	eyeRenderViewport[1].Pos = Vector2i(0, 0);
	eyeRenderViewport[1].Size = size;

	eyeTexture[0].D3D11.Header.API = ovrRenderAPI_D3D11;
	eyeTexture[0].D3D11.Header.TextureSize = size;
	eyeTexture[0].D3D11.Header.RenderViewport = eyeRenderViewport[0];
	eyeTexture[0].D3D11.pTexture = graphicsAPI_->renderTextureLeft_->renderTargetTexture_;
	eyeTexture[0].D3D11.pSRView = graphicsAPI_->renderTextureLeft_->GetShaderResourceView();

	eyeTexture[1].D3D11.Header.API = ovrRenderAPI_D3D11;
	eyeTexture[1].D3D11.Header.TextureSize = size;
	eyeTexture[1].D3D11.Header.RenderViewport = eyeRenderViewport[1];
	eyeTexture[1].D3D11.pTexture = graphicsAPI_->renderTextureRight_->renderTargetTexture_;
	eyeTexture[1].D3D11.pSRView = graphicsAPI_->renderTextureRight_->GetShaderResourceView();

	ovrHmd_EndFrame(hmd_, eyeRenderPose_, &eyeTexture[0].Texture);

	return true;
}