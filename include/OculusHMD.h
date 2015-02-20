#ifndef OCULUSHMD_H
#define OCULUSHMD_H
// #include <opencv2/core/core.hpp>
// #include "CameraInputHandler.h"
// #include "IDSuEyeInputHandler.h"
// #include "../../oculus/ovr_sdk_win_0.4.4/OculusSDK/LibOVR/Src/OVR_CAPI.h" // add oculus sdk to project libs afterwards!
#include "OVR_CAPI.h"

class OculusHMD
{
	private:
		ovrHmd oculus_device_;
		ovrSizei resolution_;
		bool running_ = false;
		static OculusHMD* instance_;

	public:
		OculusHMD();
		virtual ~OculusHMD();
		static void initialization();
		static OculusHMD* instance();

		//** - Description: gets 3 floats by reference and calculates the motion directly to them
		// @param1: y rotation
		// @param2: x rotation
		// @param3: z rotation
		// @return: void
		// --------------------
		void trackMotion(float& yaw, float& eyepitch, float& eyeroll);
};

#endif // OCULUSHMD_H
