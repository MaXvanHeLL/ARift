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
	auto_sensor_shutter[0] = false;
	auto_sensor_shutter[1] = false;
	auto_sensor_gain[0] = false;
	auto_sensor_gain[1] = false;
	cameraCaptureing_ = true;
}

IDSuEyeInputHandler::~IDSuEyeInputHandler()
{
  is_ExitCamera(m_hcam[0]);
  is_ExitCamera(m_hcam[1]);
}

bool IDSuEyeInputHandler::openCams()
{
  std::cout << "IDSuEyeInputHandler: openCams() " << std::endl;
  m_hcam[0] = m_cam1;
  m_hcam[1] = m_cam2;
  int nRet1 = is_InitCamera(&m_hcam[0], NULL);
  int nRet2 = is_InitCamera(&m_hcam[1], NULL);
  if (nRet1 != IS_SUCCESS || nRet2 != IS_SUCCESS)
  {
    if (nRet1 != IS_SUCCESS)
      std::cout << "Error could not open cam " << m_cam1 << " code " << nRet1 << std::endl;
    if (nRet2 != IS_SUCCESS)
      std::cout << "Error could not open cam " << m_cam2 << " code " << nRet2 << std::endl;
    return false;
  }

  nRet1 = is_SetColorMode(m_hcam[0], IS_CM_RGBA8_PACKED);// TODO set memory format to agree with opencv
  nRet2 = is_SetColorMode(m_hcam[1], IS_CM_RGBA8_PACKED);
  if (nRet1 != IS_SUCCESS || nRet2 != IS_SUCCESS)
  {
    std::cout << "Error could specify color formats to IS_CM_BGR8_PACKED" << std::endl;
    return false;
  }
  is_SetDisplayMode(m_hcam[0], IS_SET_DM_DIB); // no image display by the driver
  is_SetDisplayMode(m_hcam[1], IS_SET_DM_DIB);
  initMemory();

  is_SetExternalTrigger(m_hcam[0], IS_SET_TRIGGER_SOFTWARE);
  is_SetExternalTrigger(m_hcam[1], IS_SET_TRIGGER_SOFTWARE);
  std::cout << "Starting capture " << std::endl;
  nRet1 = is_CaptureVideo(m_hcam[0], IS_WAIT); // start capture and wait for first image to be in memory
  nRet2 = is_CaptureVideo(m_hcam[1], IS_WAIT);
  if (nRet1 != IS_SUCCESS || nRet2 != IS_SUCCESS)
  {
    if (nRet1 != IS_SUCCESS)
      std::cout << "Error could not capture video on cam " << m_cam1 << " code " << nRet1 << std::endl;
    if (nRet2 != IS_SUCCESS)
      std::cout << "Error could not capture video on cam " << m_cam2 << " code " << nRet2 << std::endl;
    return false;
  }
  switchAutoSensorGain(1);
  switchAutoSensorGain(2);
  switchAutoSensorShutter(1);
  switchAutoSensorShutter(2);
  // DEBUG
  double fps = 0;
  is_GetFramesPerSecond(m_hcam[0],&fps);
  std::cout << "fps: " << fps << std::endl;
  // \Debug
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
  int ret1 = is_AllocImageMem(m_hcam[cam], width, height, 32, &new_mem_addr,&new_mem_id); // 32 | 24 Bits (Alpha Channel)
  if(ret1 != IS_SUCCESS)
  {
    std::cout << "Error initializing camera memory code " <<ret1 << std::endl;
    return false;
  }
  m_cam_img_mem[cam].push_back(std::make_pair(new_mem_addr,new_mem_id));
  is_AddToSequence(m_hcam[cam],m_cam_img_mem[cam].back().first,m_cam_img_mem[cam].back().second);
  is_SetImageMem(m_hcam[cam],m_cam_img_mem[cam].back().first,m_cam_img_mem[cam].back().second);
  return true;
}

void IDSuEyeInputHandler::printMem(int cam)
{
  std::cout << "Mem stored for cam " << cam << ": " << m_cam_img_mem[cam].size() << std::endl;
  for(unsigned int mems = 0; mems < m_cam_img_mem[cam].size(); mems++)
  {
    std::cout << "m_cam_img_mem[" << cam << "][" << mems<< "] ";
    if(m_cam_img_mem[cam][mems].first == NULL)
      std::cout << "NULL, ";
    else
      std::cout << std::hex << (int)m_cam_img_mem[cam][mems].first << ", ";

    std::cout << std::hex << m_cam_img_mem[cam][mems].second << " " << std::endl;
  }

}

void IDSuEyeInputHandler::retrieveFrame(cv::Mat& frame, int cam, unsigned char* cam_buffer = NULL)
{


  if(cam != 1 && cam != 2)
  {
    std::cout << "IDSuEyeInputHandler::retrieveFrame no cam with ID " << cam << std::endl;
  }
	// added
	size_t memory_bytes = 4 * width * height * depth / 8;
  char* last_img_mem = NULL;
  is_GetActSeqBuf(m_hcam[cam-1],0,NULL,&last_img_mem);
  is_LockSeqBuf(m_hcam[cam-1],IS_IGNORE_PARAMETER,last_img_mem);
	char* driver_data = new char[memory_bytes];
	memcpy(driver_data, last_img_mem, memory_bytes);
	// [Added]
	if (cam_buffer && cameraCaptureing_)
	{
		memcpy(cam_buffer, last_img_mem, memory_bytes);
		cameraCaptureing_ = false;
		std::cout << "Camera Image Freezed!" << std::endl;
	}
	//
  is_UnlockSeqBuf(m_hcam[cam-1],IS_IGNORE_PARAMETER,last_img_mem);

	
  Mat rgb(height,width,CV_8UC4,(void*)driver_data); // CV_8UC3 | CV_8UC4 (Alpha Channel)
  if(frame.type() != rgb.type() || frame.rows != rgb.rows || frame.cols != rgb.cols )
  {
    frame = Mat::zeros(rgb.rows,rgb.cols,rgb.type());
  }
  // change format from rgb to bgr
  int from_to[] = { 0,2, 1,1, 2,0};
  mixChannels(&rgb, 1, &frame, 1, from_to,3);
  rgb.release();
  delete[] driver_data;
  if(flip_status_cam[cam - 1] != NOFLIP)
    flip(frame,frame,flip_status_cam[cam - 1]);
		
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
  if(auto_sensor_shutter[cam-1])
    enable = 0;
  else
    enable = 1;

  is_SetAutoParameter(m_hcam[cam-1],IS_SET_ENABLE_AUTO_SENSOR_SHUTTER,&enable,0);
  auto_sensor_shutter[cam-1] = !auto_sensor_shutter[cam-1];
  return auto_sensor_shutter[cam-1];
}

bool IDSuEyeInputHandler::switchAutoSensorGain(int cam)
{
  if(cam != 1 && cam != 2)
  {
    std::cout << "IDSuEyeInputHandler::setAutoSensorGain(int cam, bool status) no cam with ID " << cam << std::endl;
  }
  double enable;
  if(auto_sensor_gain[cam-1])
    enable = 0;
  else
    enable = 1;


  is_SetAutoParameter(m_hcam[cam-1],IS_SET_ENABLE_AUTO_SENSOR_GAIN,&enable,0);
  auto_sensor_gain[cam-1] = !auto_sensor_gain[cam-1];
  return auto_sensor_gain[cam-1];
}

