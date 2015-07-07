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

    void start() {running = true;};
    void stop() {running = false;};
    bool keepRunning() {return running;};

    std::string base_save_name;


		float virtualcameraX_translation = 0.0f;
		float virtualcameraY_translation = 0.0f;
		float virtualcameraZ_translation = 0.0f;

    int wait_time = 30;
    IDSuEyeInputHandler *cam_input = NULL;
    Shader::UndistortionBuffer left_cam_params_;
    Shader::UndistortionBuffer right_cam_params_;
  protected:
  private:
    bool running = false;
    int write_counter = 0;
    float step = 0.2f;
};

#endif // ARIFTCONTROL_H
