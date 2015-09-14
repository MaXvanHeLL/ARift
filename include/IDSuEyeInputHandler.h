#ifndef IDSUEYEINPUTHANDLER_H
#define IDSUEYEINPUTHANDLER_H
#include "uEye.h"
#include <vector>
#include <utility>

#define CAMERA_CHANNELS 4
#define CAMERA_WIDTH 752
#define CAMERA_HEIGHT 480
#define CAMERA_DEPTH 8
#define CAMERA_BUFFER_LENGTH ((CAMERA_CHANNELS * CAMERA_WIDTH * CAMERA_HEIGHT  * CAMERA_DEPTH) / 8)

enum FlipStatus { NOFLIP = -2, BOTH = -1, VERTICAL = 0, HORIZONTAL = 1 };

class IDSuEyeInputHandler
{
  public:
    // Methods
    IDSuEyeInputHandler();
    virtual ~IDSuEyeInputHandler();

    bool openCams(int left_cam, int right_cam);
    bool grabFrames();
    bool grabFrame(int cam);
    void retrieveFrame(int cam);
		void readFrame(int cam);
    bool switchAutoSensorShutter(int cam);
    bool switchAutoSensorGain(int cam);
    void changeAutoSensorSpeeds(double step);
    double getFrameRate(int cam);

    // Members
    unsigned char* cameraBufferLeft_;
    unsigned char* cameraBufferRight_;
    HANDLE cameraMutexLeft_;
    HANDLE cameraMutexRight_;
    FlipStatus flip_status_cam_[2];
  protected:
  private:
    // Methods
    void initMemory();
    bool addMemoryToCam(int cam);
    void printMem(int cam);
    // Members
    bool auto_sensor_shutter_[2];
		bool cameraCaptureing_;
	  bool auto_sensor_gain_[2];
    HIDS hcam_[2];
    int cam1_;
    int cam2_;
    std::vector<std::pair<char*,int > > cam_img_mem_[2];
};

#endif // IDSUEYEINPUTHANDLER_H
