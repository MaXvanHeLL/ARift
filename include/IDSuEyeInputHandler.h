#ifndef IDSUEYEINPUTHANDLER_H
#define IDSUEYEINPUTHANDLER_H
#include "uEye.h"
#include "CameraInputHandler.h"
#include <vector>
#include <utility>

class IDSuEyeInputHandler : public CameraInputHandler
{
  public:
    IDSuEyeInputHandler();
    virtual ~IDSuEyeInputHandler();

    bool openCams();
    bool grabFrames() {return true;};
    void retrieveFrame(cv::Mat& frame, int cam);
    void readFrame(cv::Mat& frame, int cam);
    int width = 752; // strange I know
    int height = 480;
    int depth = 8;
    bool switchAutoSensorShutter(int cam);
    bool switchAutoSensorGain(int cam);
  protected:
  private:
    bool auto_sensor_shutter[2];
	bool auto_sensor_gain[2];
    HIDS m_hcam[2];
    std::vector<std::pair<char*,int > > m_cam_img_mem[2];

    void initMemory();
    bool addMemoryToCam(int cam);
    void printMem(int cam);
};

#endif // IDSUEYEINPUTHANDLER_H
