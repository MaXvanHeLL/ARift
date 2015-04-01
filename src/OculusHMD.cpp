#include "../include/OculusHMD.h"
#include "../include/GraphicsAPI.h"
#include "../include/ARiftControl.h"
#include <iostream>
#include <algorithm> 
#include <d3d11.h>
#pragma comment (lib, "d3d11.lib")

using namespace OVR;
using namespace std;

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
void OculusHMD::configureStereoRendering()
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

