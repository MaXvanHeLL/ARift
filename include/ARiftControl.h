#ifndef ARIFTCONTROL_H
#define ARIFTCONTROL_H
#include "IDSuEyeInputHandler.h"
#include "../include/Shader.h"
#include "../include/GraphicsAPI.h"
#include "../include/Model.h"

#include <direct.h>
#include <chrono>
#include <ctime>
#include <iostream>

#define CAM1 1
#define CAM2 2
#define RIFT_RESOLUTION_WIDTH 1280
#define RIFT_RESOLUTION_HEIGHT 800
//#define RIFT_RESOLUTION_WIDTH (1122 * 2)
//#define RIFT_RESOLUTION_HEIGHT 1553

/// <summary>
/// Class for handling and interpreting keyboard and mouse input.
/// <\summary>
class ARiftControl
{
  public:
    enum InputMode { DEFAULT = 0, MODEL = 1, WORLD = 2, CAMERA = 3, MOVEMENT = 4};

    ARiftControl();
    virtual ~ARiftControl();
    void init(GraphicsAPI* graphicsAPI);
    void getImages();

    void handleKey(char key);
    void hanldeFlip();
    void handleSave();
    void handleCameraAutoFeatures();

    void start() {running_ = true;};
    void stop() {running_ = false;};
    bool keepRunning() {return running_;};

    std::string baseSaveName_;

    int minInterKeyPressDelay = 30;
    IDSuEyeInputHandler *camInput_ = NULL;
    Shader::UndistortionBuffer leftCameraParameters_;
    Shader::UndistortionBuffer rightCameraParameters_;

    Model::State oldModelState_;
    Model::State newModelState_;

    float worldOffsetX_ = -2.4f;
    float worldOffsetY_ = 4.6f;
    float worldOffsetZ_ = 9.2f;

    float headToEyeOffsetX_ = 0.032f;
    float headToEyeOffsetY_ = -0.28f;
    float headToEyeOffsetZ_ = 0.936f;
    float interPupillaryOffset_ = 0.064f;
    
    float cameraPositionX_ = 0.0f;
    float cameraPositionY_ = 0.0f;
    float cameraPositionZ_ = -10.0f;

    InputMode inputMode_ = InputMode::DEFAULT;
    GraphicsAPI* graphicsAPI_ = NULL;
  protected:
  private:
    bool running_ = false;
    int writeCounter_ = 0;
    float step_ = 0.2f;
    char lastKey_ = 0; 
    std::chrono::time_point<std::chrono::system_clock> lastKeyTime;
    std::chrono::time_point<std::chrono::system_clock> programStartTime;
    std::chrono::duration<double> minimumKeyInputDelay;
};

#endif // ARIFTCONTROL_H
