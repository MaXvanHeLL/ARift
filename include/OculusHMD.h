#ifndef OCULUSHMD_H
#define OCULUSHMD_H
// #define OVR_D3D_VERSION 11
#include <OVR_CAPI.h>
// #include "OVR_CAPI_D3D.h"
#include "Kernel/OVR_Math.h"

using namespace OVR;

class GraphicsAPI;

class OculusHMD
{
	private:
		static OculusHMD* instance_;
		GraphicsAPI* graphicsAPI_;
		bool running_ = false;

	public:
		ovrHmd hmd_;
		ovrSizei resolution_;
		Sizei eyeSize_[2];
		ovrEyeRenderDesc eyeRenderDesc_[2];
		ovrVector3f useHmdToEyeViewOffset_[2];
		ovrPosef eyeRenderPose_[2];
		float initPitch_;

		OculusHMD();
		virtual ~OculusHMD();

		//** - Description: used for creating the Singleton Object once
		// @return: void
		// ----
		static void initialization(GraphicsAPI*);

		//** - Description: As a Singleton class, this static function returns the object instance of OculusHMD
		// @return: the OculusHMD* object instance
		// ----
		static OculusHMD* instance();

		//** - Description: calculate FOV size
		// @return: void
		// ----
		void calculateFOV();

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

		void StartFrames();

		bool RenderDistortion();
    bool Recenter();
};

#endif // OCULUSHMD_H
