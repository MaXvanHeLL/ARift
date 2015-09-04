#include <iostream>
#include <utility>


#include "../include/IDSuEyeInputHandler.h"
#include "uEye.h"

IDSuEyeInputHandler::IDSuEyeInputHandler()
{
	auto_sensor_shutter_[0] = false;
	auto_sensor_shutter_[1] = false;
	auto_sensor_gain_[0] = false;
	auto_sensor_gain_[1] = false;
	cameraCaptureing_ = false;
  cameraBufferLeft_ = new unsigned char[CAMERA_BUFFER_LENGTH];
  cameraBufferRight_ = new unsigned char[CAMERA_BUFFER_LENGTH];
  cameraMutexLeft_ = CreateMutex(NULL, FALSE, L"Camera Left Mutex");
  cameraMutexRight_ = CreateMutex(NULL, FALSE, L"Camera Right Mutex");
  if (cameraMutexLeft_ == NULL || cameraMutexRight_ == NULL)
  {
    std::cout << "Create Mutex error!" << std::endl;
  }
  flip_status_cam_[0] = NOFLIP;
  flip_status_cam_[1] = BOTH;
}

IDSuEyeInputHandler::~IDSuEyeInputHandler()
{
  is_ExitCamera(hcam_[0]);
  is_ExitCamera(hcam_[1]);
}

bool IDSuEyeInputHandler::openCams(int left_cam,int right_cam)
{
  std::cout << "IDSuEyeInputHandler: openCams() " << std::endl;
  cam1_ = left_cam;
  cam2_ = right_cam;
  hcam_[0] = left_cam;
  hcam_[1] = right_cam;
  int nRet1 = is_InitCamera(&hcam_[0], NULL);
  int nRet2 = is_InitCamera(&hcam_[1], NULL);
  if (nRet1 != IS_SUCCESS || nRet2 != IS_SUCCESS)
  {
    if (nRet1 != IS_SUCCESS)
      std::cout << "IDSuEyeInputHandler: Error could not open cam " << cam1_ << " code " << nRet1 << std::endl;
    if (nRet2 != IS_SUCCESS)
      std::cout << "IDSuEyeInputHandler: Error could not open cam " << cam2_ << " code " << nRet2 << std::endl;
    return false;
  }

  nRet1 = is_SetColorMode(hcam_[0], IS_CM_RGBA8_PACKED);
  nRet2 = is_SetColorMode(hcam_[1], IS_CM_RGBA8_PACKED);
  if (nRet1 != IS_SUCCESS || nRet2 != IS_SUCCESS)
  {
    std::cout << "IDSuEyeInputHandler: Error could specify color formats to IS_CM_BGR8_PACKED" << std::endl;
    return false;
  }
  is_SetDisplayMode(hcam_[0], IS_SET_DM_DIB); // no image display by the driver
  is_SetDisplayMode(hcam_[1], IS_SET_DM_DIB);
  initMemory();

  is_SetExternalTrigger(hcam_[0], IS_SET_TRIGGER_SOFTWARE);
  is_SetExternalTrigger(hcam_[1], IS_SET_TRIGGER_SOFTWARE);
  std::cout << "IDSuEyeInputHandler: Starting capture " << std::endl;
  nRet1 = is_CaptureVideo(hcam_[0], IS_WAIT); // start capture and wait for first image to be in memory
  nRet2 = is_CaptureVideo(hcam_[1], IS_WAIT);
  if (nRet1 != IS_SUCCESS || nRet2 != IS_SUCCESS)
  {
    if (nRet1 != IS_SUCCESS)
      std::cout << "IDSuEyeInputHandler: Error could not capture video on cam " << cam1_ << " code " << nRet1 << std::endl;
    if (nRet2 != IS_SUCCESS)
      std::cout << "IDSuEyeInputHandler: Error could not capture video on cam " << cam2_ << " code " << nRet2 << std::endl;
    return false;
  }
  switchAutoSensorGain(1);
  switchAutoSensorGain(2);
  switchAutoSensorShutter(1);
  switchAutoSensorShutter(2);
  double auto_fps0 = -1.0;
  double auto_fps1 = -1.0;
  is_SetAutoParameter(hcam_[0], IS_GET_ENABLE_AUTO_FRAMERATE, &auto_fps0, 0);
  is_SetAutoParameter(hcam_[1], IS_GET_ENABLE_AUTO_FRAMERATE, &auto_fps1, 0);
  std::cout << "IDSuEyeInputHandler: auto fps status cam 0 :" << auto_fps0 << " cam 1: " << auto_fps1 << std::endl;
  double fps, new_fps0, new_fps1;
  fps = 30;
  new_fps0 = new_fps1 = -1.0;
  cameraCaptureing_ = true;
  return true;
}

void IDSuEyeInputHandler::initMemory()
{
  bool ret1 = true;
  bool ret2 = true;
  for(int mem_count_per_cam = 0; mem_count_per_cam < 3 && ret1 && ret2; mem_count_per_cam++)
  {
    ret1 = addMemoryToCam(0);
    ret2 = addMemoryToCam(1);
  }
}

bool IDSuEyeInputHandler::addMemoryToCam(int cam)
{
  char *new_mem_addr;
  int new_mem_id;
  int ret1 = is_AllocImageMem(hcam_[cam], CAMERA_WIDTH, CAMERA_HEIGHT, CAMERA_DEPTH*CAMERA_CHANNELS, &new_mem_addr, &new_mem_id); // 32 | 24 Bits (Alpha Channel)
  if(ret1 != IS_SUCCESS)
  {
    std::cout << "IDSuEyeInputHandler: Error initializing camera memory code " <<ret1 << std::endl;
    return false;
  }
  cam_img_mem_[cam].push_back(std::make_pair(new_mem_addr,new_mem_id));
  is_AddToSequence(hcam_[cam], cam_img_mem_[cam].back().first, cam_img_mem_[cam].back().second);
  is_SetImageMem(hcam_[cam], cam_img_mem_[cam].back().first, cam_img_mem_[cam].back().second);
  return true;
}

void IDSuEyeInputHandler::printMem(int cam)
{
  std::cout << "IDSuEyeInputHandler: Mem stored for cam " << cam << ": " << cam_img_mem_[cam].size() << std::endl;
  for (unsigned int mems = 0; mems < cam_img_mem_[cam].size(); mems++)
  {
    std::cout << "m_cam_img_mem[" << cam << "][" << mems<< "] ";
    if (cam_img_mem_[cam][mems].first == NULL)
      std::cout << "NULL, ";
    else
      std::cout << std::hex << (int)cam_img_mem_[cam][mems].first << ", ";

    std::cout << std::hex << cam_img_mem_[cam][mems].second << " " << std::endl;
  }

}
bool IDSuEyeInputHandler::grabFrames()
{
  bool result1 = grabFrame(cam1_);
  bool result2 = grabFrame(cam2_);
  return (result2 && result1);
}

bool IDSuEyeInputHandler::grabFrame(int cam)
{
  char* last_img_mem = NULL;
  char* driver_data = new char[CAMERA_BUFFER_LENGTH];
  is_GetActSeqBuf(hcam_[cam - 1],0,NULL,&last_img_mem);
  is_LockSeqBuf(hcam_[cam - 1], IS_IGNORE_PARAMETER, last_img_mem);
  memcpy(driver_data, last_img_mem, CAMERA_BUFFER_LENGTH);
  is_UnlockSeqBuf(hcam_[cam - 1], IS_IGNORE_PARAMETER, last_img_mem);

  if (cam == 1)
  { 
    WaitForSingleObject(cameraMutexLeft_, INFINITE);
    // no flip needed
    memcpy(cameraBufferLeft_, driver_data, CAMERA_BUFFER_LENGTH);
    ReleaseMutex(cameraMutexLeft_);
  } 
  else 
  {
    WaitForSingleObject(cameraMutexRight_, INFINITE);
    // flip image
    unsigned char *buffer = cameraBufferRight_;
    char *driver_buffer = driver_data + CAMERA_BUFFER_LENGTH;
    int byte_per_pixel = (CAMERA_CHANNELS * CAMERA_DEPTH) / 8;
    for (int pixel_id = 0; pixel_id < CAMERA_WIDTH * CAMERA_HEIGHT; pixel_id++)
    {
      memcpy(buffer, driver_buffer, byte_per_pixel);
      buffer += byte_per_pixel;
      driver_buffer -= byte_per_pixel;
    }
    ReleaseMutex(cameraMutexRight_);
  }
  delete[] driver_data;
  return true;
}

void IDSuEyeInputHandler::retrieveFrame(int cam)
{
  if(cam != cam1_ && cam != cam2_)
  {
    std::cout << "IDSuEyeInputHandler::retrieveFrame no cam with ID " << cam << std::endl;
  }

  grabFrame(cam);
}

void IDSuEyeInputHandler::readFrame(int cam)
{
  retrieveFrame(cam);
}

bool IDSuEyeInputHandler::switchAutoSensorShutter(int cam)
{
  if(cam != 1 && cam != 2)
  {
    std::cout << "IDSuEyeInputHandler::setAutoSensorShutter(int cam, bool status) no cam with ID " << cam << std::endl;
  }

  double enable;
  if(auto_sensor_shutter_[cam-1])
    enable = 0;
  else
    enable = 1;

  is_SetAutoParameter(hcam_[cam-1],IS_SET_ENABLE_AUTO_SENSOR_SHUTTER,&enable,0);
  auto_sensor_shutter_[cam-1] = !auto_sensor_shutter_[cam-1];
  return auto_sensor_shutter_[cam-1];
}

bool IDSuEyeInputHandler::switchAutoSensorGain(int cam)
{
  if(cam != 1 && cam != 2)
  {
    std::cout << "IDSuEyeInputHandler::setAutoSensorGain(int cam, bool status) no cam with ID " << cam << std::endl;
  }
  double enable;
  if(auto_sensor_gain_[cam-1])
    enable = 0;
  else
    enable = 1;


  is_SetAutoParameter(hcam_[cam - 1], IS_SET_ENABLE_AUTO_SENSOR_GAIN, &enable, 0);
  auto_sensor_gain_[cam-1] = !auto_sensor_gain_[cam-1];
  return auto_sensor_gain_[cam-1];
}

void IDSuEyeInputHandler::changeAutoSensorSpeeds(double step)
{
  double auto_speed0 = -1.0f;
  double auto_speed1 = -1.0f; // IS_GET_AUTO_SPEED
  is_SetAutoParameter(hcam_[0], IS_GET_AUTO_SPEED, &auto_speed0, 0);
  is_SetAutoParameter(hcam_[1], IS_GET_AUTO_SPEED, &auto_speed1, 0);
  std::cout << "IDSuEyeInputHandler::changeAutoSensorSpeed speeds are" << std::endl << " (l , r): (" << auto_speed0 << " , " << auto_speed1 << ") += " << step << std::endl;
  auto_speed0 += step;
  auto_speed1 += step;
  if (auto_speed0 < 0.0f || auto_speed1 < 0.0f || auto_speed0 > 100.0f || auto_speed1 > 100.0f )
  {
    std::cout << "IDSuEyeInputHandler::changeAutoSensorSpeed ERROR new auto speed out of bounds (0..100) abroting"<< std::endl;
    return;
  }
  is_SetAutoParameter(hcam_[0], IS_SET_AUTO_SPEED, &auto_speed0, 0);
  is_SetAutoParameter(hcam_[1], IS_SET_AUTO_SPEED, &auto_speed1, 0);
}

double IDSuEyeInputHandler::getFrameRate(int cam)
{
  double fps = -1.0f;
  is_GetFramesPerSecond(hcam_[cam - 1], &fps);
  return fps;
}