#ifndef OCULUSHMD_H
#define OCULUSHMD_H
#include <opencv2/core/core.hpp>
// #include "CameraInputHandler.h"
// #include "IDSuEyeInputHandler.h"
// #include "../../oculus/ovr_sdk_win_0.4.4/OculusSDK/LibOVR/Src/OVR_CAPI.h" // add oculus sdk to project libs afterwards!
#include "OVR_CAPI.h"

class OculusHMD
{
	private:
		static OculusHMD* instance_;
		ovrHmd hmd_;
		ovrSizei resolution_;
		bool running_ = false;

	public:
		OculusHMD();
		virtual ~OculusHMD();

		//** - Description: used for creating the Singleton Object once
		// @return: void
		// ----
		static void initialization();

		//** - Description: As a Singleton class, this static function returns the object instance of OculusHMD
		// @return: the OculusHMD* object instance
		// ----
		static OculusHMD* instance();

		//** - Description: Setting up the stereo rendering configuration for the HMD during initialization()
		// @return: void
		// ----
		void configureStereoRendering();

		//** - Description: gets 3 floats by reference and calculates the motion directly to them
		// @param1: y rotation
		// @param2: x rotation
		// @param3: z rotation
		// @return: void
		// ----
		void trackMotion(float& yaw, float& eyepitch, float& eyeroll);

		//** - Description: Renders The Buffer to each of the HMD Screens
		// @param1: y rotation
		// @param2: x rotation
		// @param3: z rotation
		// @return: void
		// ----
		void render(cv::Mat, cv::Mat);
};

#endif // OCULUSHMD_H
