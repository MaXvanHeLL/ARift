#ifndef ARIFTCONTROL_H
#define ARIFTCONTROL_H
#include <opencv2/core/core.hpp>
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
    bool getImages();
    void createDisplay();

    void handleKey(char key);
    void hanldeFlip();
    void handleSave();
    void handleCameraAutoFeatures();

    void start() {running = true;};
    void stop() {running = false;};
    bool keepRunning() {return running;};

    void undistortImages();

    cv::Mat left_pic;
    cv::Mat right_pic;
    cv::Mat left_resized;
    cv::Mat right_resized;
    cv::Mat left_undistorted;
    cv::Mat right_undistorted;
    cv::Mat full_view;
    cv::Mat full_view_undist;
    std::string base_save_name;
		// [Stuff for other DirectX Thread]
		cv::Mat picture_1_;
		cv::Mat picture_2_;
		// --------------------------------
    cv::Mat left_camera_mat;
    cv::Mat right_camera_mat;
    cv::Mat left_distortion;
    cv::Mat right_distortion;
    cv::Mat left_map1;
    cv::Mat left_map2;
    cv::Mat right_map1;
    cv::Mat right_map2;
    int wait_time = 30;
    IDSuEyeInputHandler *cam_input = NULL;
    Shader::UndistortionBuffer left_cam_params_;
    Shader::UndistortionBuffer right_cam_params_;
  protected:
  private:
    bool running = false;
    int write_counter = 0;
};

#endif // ARIFTCONTROL_H
