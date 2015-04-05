#ifndef IDSUEYEINPUTHANDLER_H
#define IDSUEYEINPUTHANDLER_H
#include "uEye.h"
#include "CameraInputHandler.h"
#include <vector>
#include <utility>

#define CAMERA_CHANNELS 4
#define CAMERA_WIDTH 752
#define CAMERA_HEIGHT 480
#define CAMERA_DEPTH 8
#define CAMERA_BUFFER_LENGTH (CAMERA_CHANNELS * CAMERA_WIDTH * CAMERA_HEIGHT  * CAMERA_DEPTH / 8)

class IDSuEyeInputHandler : public CameraInputHandler
{
  public:
    IDSuEyeInputHandler();
    virtual ~IDSuEyeInputHandler();

    bool openCams();
    bool grabFrames() {return true;};
    void retrieveFrame(cv::Mat& frame, int cam);
		void readFrame(cv::Mat& frame, int cam);

    // Members
    int width = 752; // strange I know
    int height = 480;
    int depth = 8;
    bool switchAutoSensorShutter(int cam);
    bool switchAutoSensorGain(int cam);
    unsigned char* cameraBufferLeft_;
    unsigned char* cameraBufferRight_;
    HANDLE cameraMutexLeft_;
    HANDLE cameraMutexRight_;
  protected:
  private:
    bool auto_sensor_shutter[2];
		bool cameraCaptureing_;
	  bool auto_sensor_gain[2];
    HIDS m_hcam[2];
    std::vector<std::pair<char*,int > > m_cam_img_mem[2];

    void initMemory();
    bool addMemoryToCam(int cam);
    void printMem(int cam);
};

#endif // IDSUEYEINPUTHANDLER_H
