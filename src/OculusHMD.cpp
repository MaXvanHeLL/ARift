// #include "../include/CameraInputHandler.h"
// #include "../include/IDSuEyeInputHandler.h"
// #include "../include/ARiftControl.h"
#include "../include/OculusHMD.h"
#include "Kernel\OVR_Math.h"
#include <iostream>
#include <algorithm> 
#include <d3d11.h>
#pragma comment (lib, "d3d11.lib")
#include "DirectXRendering.cpp"
// #include "../../oculus/ovr_sdk_win_0.4.4/OculusSDK/Samples/CommonSrc/Render/Render_Device.h"

// #include "Util\Util_Render_Stereo.h"
// #include <d3dcompiler.h>

/*
#define OVR_D3D_VERSION 11
#define OVR_OS_WIN32 1
#include "OVR_CAPI_D3D.h"
*/

// #include <d3dcompiler.h>
// #include "include/Helpers.h"
// #include <opencv2/core/core.hpp>
// #include <opencv2/calib3d/calib3d.hpp>
// #include <direct.h>

// #define GetCurrentDir _getcwd

using namespace OVR;
using namespace std;

OculusHMD* OculusHMD::instance_ = NULL;

/*
ovrEyeRenderDesc EyeRenderDesc[2];     // Description of the VR.
ovrRecti         EyeRenderViewport[2]; // Useful to remember when varying resolution
ImageBuffer    * pEyeRenderTexture[2]; // Where the eye buffers will be rendered
ImageBuffer    * pEyeDepthBuffer[2];   // For the eye buffers to use when rendered
*/

// -----------------------------------------------------------------------     
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
		configureStereoRendering();
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
void OculusHMD::initialization()
{
	if (!instance_)
		instance_ = new OculusHMD();
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
void OculusHMD::configureStereoRendering()
{
	if (hmd_)
	{
		// ------- | OLD STUFF | ------------- ********
		Sizei recommendedTex0Size = ovrHmd_GetFovTextureSize(hmd_, ovrEye_Left, hmd_->DefaultEyeFov[0], 1.0f);
		Sizei recommendedTex1Size = ovrHmd_GetFovTextureSize(hmd_, ovrEye_Right, hmd_->DefaultEyeFov[1], 1.0f);

		Sizei renderTargetSize;
		renderTargetSize.w = recommendedTex0Size.w + recommendedTex1Size.w;
		renderTargetSize.h = max(recommendedTex0Size.h, recommendedTex1Size.h);

		const int eyeRenderMultisample = 1;
		
		// pDevice->CreateTexture2D
		
		// Make the eye render buffers (caution if actual size < requested due to HW limits). 

		/* ------- | NEW STUFF | ------------- ********
	    for (int eye = 0; eye<2; eye++)
		{
			Sizei idealSize = ovrHmd_GetFovTextureSize(hmd_, (ovrEyeType)eye,
				hmd_->DefaultEyeFov[eye], 1.0f);

			pEyeRenderTexture[eye] = new ImageBuffer(true, false, idealSize);
			pEyeDepthBuffer[eye] = new ImageBuffer(true, true, pEyeRenderTexture[eye]->Size);
			EyeRenderViewport[eye].Pos = Vector2i(0, 0);
			EyeRenderViewport[eye].Size = pEyeRenderTexture[eye]->Size;
		}

		// Setup VR components
		ovrD3D11Config d3d11cfg;
		d3d11cfg.D3D11.Header.API = ovrRenderAPI_D3D11;
		d3d11cfg.D3D11.Header.BackBufferSize = Sizei(hmd_->Resolution.w, hmd_->Resolution.h);
		d3d11cfg.D3D11.Header.Multisample = 1;
		d3d11cfg.D3D11.pDevice = DX11.Device;
		d3d11cfg.D3D11.pDeviceContext = DX11.Context;
		d3d11cfg.D3D11.pBackBufferRT = DX11.BackBufferRT;
		d3d11cfg.D3D11.pSwapChain = DX11.SwapChain;

		ovrHmd_ConfigureRendering(hmd_, &d3d11cfg.Config, ovrDistortionCap_Chromatic | ovrDistortionCap_Vignette |
			ovrDistortionCap_TimeWarp | ovrDistortionCap_Overdrive, hmd_->DefaultEyeFov, EyeRenderDesc);

		// ovrHmd_AttachToWindow(hmd_, DX11.Window, NULL, NULL);	
		*/
	}
}

void OculusHMD::render(cv::Mat left_cam, cv::Mat right_cam)
{
	// beginning of rendering a Frame
	if (hmd_)
	{
		/*		// --------------- |  OLD | ------------------- *************************
		ovrHmd_BeginFrame(hmd_, 0);
		ovrVector3f useHmdToEyeViewOffset[2] = { EyeRenderDesc[0].HmdToEyeViewOffset, EyeRenderDesc[1].HmdToEyeViewOffset };

		// Get both eye poses simultaneously, with IPD offset already included. 
		ovrPosef temp_EyeRenderPose[2];
		ovrHmd_GetEyePoses(hmd_, 0, useHmdToEyeViewOffset, temp_EyeRenderPose, NULL);

		// distort the pictures
		ovrD3D11Texture eyeTexture[2]; // Gather data for eye textures 
		for (int eye = 0; eye<2; eye++)
		{
			// eyeTexture[eye].D3D11.Header.API = ovrRenderAPI_D3D11;
			eyeTexture[eye].D3D11.Header.TextureSize = pEyeRenderTexture[eye]->Size;
			eyeTexture[eye].D3D11.Header.RenderViewport = EyeRenderViewport[eye];
			eyeTexture[eye].D3D11.pTexture = pEyeRenderTexture[eye]->Tex;
			eyeTexture[eye].D3D11.pSRView = pEyeRenderTexture[eye]->TexSv;
		}
		ovrHmd_EndFrame(hmd_, 0, &eyeTexture[0].Texture);
		*/

		// --------------- |  NEW | ------------------- *************************

	}
}


