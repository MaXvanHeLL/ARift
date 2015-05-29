#include "../include/OculusHMD.h"
#include "../include/GraphicsAPI.h"
#include "../include/ARiftControl.h"
#include <iostream>
#include <algorithm> 
#define   OVR_D3D_VERSION 11
#include "OVR_CAPI_D3D.h" 
#include "../include/Helpers.h"

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

			initPitch_ = 0.0f;
		}
		else
		{
			initPitch_ = 0.0f;
      std::cout << "ERROR: OculusHMD constructor could not create HMD" << std::endl;
			// TODO: manage error handling
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
      yaw = RadToDegree(yaw);
      eyepitch = RadToDegree(eyepitch);
      eyeroll = RadToDegree(eyeroll);
      // cout << "roll: " << RadToDegree(eyeroll) << endl << endl;
    }
  }
}
void OculusHMD::updateEyePoses()
{
  ovrHmd_GetEyePoses(hmd_, 0, useHmdToEyeViewOffset_, eyeRenderPose_, NULL);
}

void OculusHMD::printEyePoses()
{
  // I am quite sure eyeRenderPose.Positiom stores the position of the eyes with respect to the rotation center in meters
  float yaw, pitch, roll;
  yaw = pitch = roll = 0.0f;
  getEulerAngles(eyeRenderPose_[0].Orientation, yaw, pitch, roll);
  std::cout << "Eyepose 0: position    ( " << eyeRenderPose_[0].Position.x << ", " << eyeRenderPose_[0].Position.y << ", " << eyeRenderPose_[0].Position.z << ") " << std::endl;
  std::cout << "           orientation ( " << eyeRenderPose_[0].Orientation.x << ", " << eyeRenderPose_[0].Orientation.y << ", " << eyeRenderPose_[0].Orientation.z << ", " << eyeRenderPose_[0].Orientation.w << ") " << std::endl;
  std::cout << "                 euler ( " << (yaw*360.0 / (2.0*3.1415926)) << ", " << (pitch*360.0 / (2.0*3.1415926)) << ", " << (roll*360.0 / (2.0*3.1415926)) << ") " << std::endl;
  std::cout << "Eyepose 1: position    ( " << eyeRenderPose_[1].Position.x << ", " << eyeRenderPose_[1].Position.y << ", " << eyeRenderPose_[1].Position.z << ") " << std::endl;
  std::cout << "           orientation ( " << eyeRenderPose_[1].Orientation.x << ", " << eyeRenderPose_[1].Orientation.y << ", " << eyeRenderPose_[1].Orientation.z << ", " << eyeRenderPose_[1].Orientation.w << ") " << std::endl << std::endl;
}

void OculusHMD::getLeftEyePose(float& x, float& y, float& z, float& pitch, float& yaw, float& roll)
{
  getEulerAngles(eyeRenderPose_[0].Orientation, pitch, yaw, roll);
  x = eyeRenderPose_[0].Position.x;
  y = eyeRenderPose_[0].Position.y;
  z = eyeRenderPose_[0].Position.z;
}
void OculusHMD::getRightEyePose(float& x, float& y, float& z, float& pitch, float& yaw, float& roll)
{
  getEulerAngles(eyeRenderPose_[1].Orientation, pitch, yaw, roll);
  x = eyeRenderPose_[1].Position.x;
  y = eyeRenderPose_[1].Position.y;
  z = eyeRenderPose_[1].Position.z;
}
void OculusHMD::getEulerAngles(ovrQuatf q, float& pitch, float& yaw, float& roll )
{
  roll = atan2(2.0f*(q.x*q.y + q.z*q.w), 1.0f - 2.0f*(q.y*q.y + q.z*q.z));
  yaw = asin(2.0f*(q.x*q.z - q.w*q.y));
  pitch = atan2(2.0f*(q.x*q.w + q.y*q.z), 1.0f - 2.0f*(q.z*q.z + q.w*q.w));
}

bool OculusHMD::Recenter()
{
  if (!hmd_)
    return false;
  ovrHmd_RecenterPose(hmd_);
  return true;
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
		std::cout << "OculusHMD::calculateFOV() | TextureHeight LeftEye: " << eyeSize_[0].h << std::endl;
		std::cout << "OculusHMD::calculateFOV() | TextureWidth LeftEye: " << eyeSize_[0].w << std::endl;
		std::cout << "OculusHMD::calculateFOV() | TextureHeight RightEye: " << eyeSize_[1].h << std::endl;
		std::cout << "OculusHMD::calculateFOV() | TextureWidth RightEye: " << eyeSize_[1].w << std::endl;
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
//		ovrDistortionCap_Chromatic | ovrDistortionCap_Vignette |
//    ovrDistortionCap_TimeWarp | ovrDistortionCap_Overdrive,
    ovrDistortionCap_Chromatic | ovrDistortionCap_Overdrive,
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
	// size.w = eyeSize_[0].w; // used for Oculus 3D Vision
	// size.h = eyeSize_[0].h; // used for Oculus 3D Vision

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