#include "../include/CameraInputHandler.h"
#include "../include/IDSuEyeInputHandler.h"
#include "../include/ARiftControl.h"
#include "../include/OculusHMD.h"
// #include "include/Helpers.h"
// #include <opencv2/core/core.hpp>
// #include <opencv2/calib3d/calib3d.hpp>
// #include <iostream>
// #include <direct.h>

// #define GetCurrentDir _getcwd

// using namespace cv;

OculusHMD::OculusHMD()
{}

OculusHMD::~OculusHMD()
{
	// ovrHmd_Destroy(oculus_device);
	// ovr_Shutdown();
}

void OculusHMD::initialization()
{
	ovr_Initialize();

	
	oculus_device = ovrHmd_Create(0); // (0) means: create first available HMD
	if (oculus_device)
	{
		resolution = oculus_device->Resolution;

		// setup for sensors and motion tracking
		ovrHmd_ConfigureTracking(oculus_device, ovrTrackingCap_Orientation | ovrTrackingCap_MagYawCorrection |
			ovrTrackingCap_Position, 0);

	}
	else
	{
		// do error stuff here
	}
}


