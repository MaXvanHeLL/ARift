#include <iostream>
#include <utility>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>

#include "../include/IDSuEyeInputHandler.h"
#include "uEye.h"

using namespace cv;

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

  nRet1 = is_SetColorMode(hcam_[0], IS_CM_RGBA8_PACKED);// TODO set memory format to agree with opencv
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
    memcpy(cameraBufferLeft_, driver_data, CAMERA_BUFFER_LENGTH);
    ReleaseMutex(cameraMutexLeft_);
  } 
  else 
  {
    WaitForSingleObject(cameraMutexRight_, INFINITE);
    memcpy(cameraBufferRight_, driver_data, CAMERA_BUFFER_LENGTH);
    ReleaseMutex(cameraMutexRight_);
  }
  delete[] driver_data;
  return true;
}

void IDSuEyeInputHandler::retrieveFrame(cv::Mat& frame, int cam)
{
  if(cam != cam1_ && cam != cam2_)
  {
    std::cout << "IDSuEyeInputHandler::retrieveFrame no cam with ID " << cam << std::endl;
  }

  grabFrame(cam);
  void* driver_data;
  if (cam == 1)
    driver_data = (void*)cameraBufferLeft_;
  else
    driver_data = (void*)cameraBufferRight_;

  Mat rgb(CAMERA_HEIGHT,CAMERA_WIDTH,CV_8UC4,driver_data); // CV_8UC3 | CV_8UC4 (Alpha Channel)
  if(frame.type() != rgb.type() || frame.rows != rgb.rows || frame.cols != rgb.cols )
  {
    frame = Mat::zeros(rgb.rows,rgb.cols,rgb.type());
  }
  // change format from rgb to bgr
  int from_to[] = { 0,2, 1,1, 2,0};
  mixChannels(&rgb, 1, &frame, 1, from_to,3);
  rgb.release();
  
  if(flip_status_cam_[cam - 1] != NOFLIP)
    flip(frame,frame,flip_status_cam_[cam - 1]);
		
}

void IDSuEyeInputHandler::readFrame(cv::Mat& frame, int cam)
{
  retrieveFrame(frame,cam);
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

