// #include "../include/CameraInputHandler.h"
// #include "../include/IDSuEyeInputHandler.h"
// #include "../include/ARiftControl.h"
#include "../include/OculusHMD.h"
#include "Kernel\OVR_Math.h"
#include <iostream>

// #include "include/Helpers.h"
// #include <opencv2/core/core.hpp>
// #include <opencv2/calib3d/calib3d.hpp>

// #include <direct.h>

// #define GetCurrentDir _getcwd
using namespace OVR;
using namespace std;

OculusHMD* OculusHMD::instance_ = NULL;

// -----------------------------------------------------------------------     
OculusHMD::OculusHMD()
{
	if (!instance_)
	{
		ovr_Initialize();

		oculus_device_ = ovrHmd_Create(0); // (0) means: create first available HMD - indexed access
		if (oculus_device_)
		{
			resolution_ = oculus_device_->Resolution;

			// setup for sensors and motion tracking
			ovrHmd_ConfigureTracking(oculus_device_, ovrTrackingCap_Orientation | ovrTrackingCap_MagYawCorrection |
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
	ovrHmd_Destroy(oculus_device_);
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
	ovrTrackingState tracking_state = ovrHmd_GetTrackingState(oculus_device_, ovr_GetTimeInSeconds());

	if (tracking_state.StatusFlags & (ovrStatus_OrientationTracked | ovrStatus_PositionTracked))
	{
		OVR::Posef pose = tracking_state.HeadPose.ThePose;
		pose.Rotation.GetEulerAngles<Axis_Y, Axis_X, Axis_Z>(&yaw, &eyepitch, &eyeroll);

		// for debug purposes only
		cout << "yaw: " << RadToDegree(yaw) << endl;
		cout << "pitch: " << RadToDegree(eyepitch) << endl;
		cout << "roll: " << RadToDegree(eyeroll) << endl;
	}
}


