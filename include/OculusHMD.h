#ifndef OCULUSHMD_H
#define OCULUSHMD_H
// #include <opencv2/core/core.hpp>
// #include "CameraInputHandler.h"
// #include "IDSuEyeInputHandler.h"
// #include "../../oculus/ovr_sdk_win_0.4.4/OculusSDK/LibOVR/Src/OVR_CAPI.h" // add oculus sdk to project libs afterwards!
#include "OVR_CAPI.h"

class OculusHMD
{
public:
	OculusHMD();
	virtual ~OculusHMD();
	void initialization();
	static OculusHMD& instance();
	
private:
	ovrHmd oculus_device;
	ovrSizei resolution;
	bool running = false;
};

#endif // OCULUSHMD_H
