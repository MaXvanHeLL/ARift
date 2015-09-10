#ifndef ARIFTCONTROL_H
#define ARIFTCONTROL_H
#include "IDSuEyeInputHandler.h"
#include "../include/Shader.h"
#include <direct.h>

#define CAM1 1
#define CAM2 2
#define RIFT_RESOLUTION_WIDTH 1280
#define RIFT_RESOLUTION_HEIGHT 800
//#define RIFT_RESOLUTION_WIDTH (1122 * 2)
//#define RIFT_RESOLUTION_HEIGHT 1553


class ARiftControl
{
  public:
    ARiftControl();
    virtual ~ARiftControl();
    void init();
    void getImages();

    void handleKey(char key);
    void hanldeFlip();
    void handleSave();
    void handleCameraAutoFeatures();

    void start() {running_ = true;};
    void stop() {running_ = false;};
    bool keepRunning() {return running_;};

    std::string base_save_name_;


		float virtualcameraX_translation_ = 0.0f;
		float virtualcameraY_translation_ = 0.0f;
		float virtualcameraZ_translation_ = 0.0f;

    int wait_time_ = 30;
    IDSuEyeInputHandler *cam_input_ = NULL;
    Shader::UndistortionBuffer left_cam_params_;
    Shader::UndistortionBuffer right_cam_params_;
  protected:
  private:
    bool running_ = false;
    int write_counter_ = 0;
    float step_ = 0.2f;
};

#endif // ARIFTCONTROL_H
