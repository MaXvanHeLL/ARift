#include "../include/OpenCVCameraInputHandler.h"
#include <iostream>

OpenCVCameraInputHandler::OpenCVCameraInputHandler() : m_cam1(0), m_cam2(1)
{
	flip_status_cam[0] = NOFLIP;
	flip_status_cam[1] = BOTH;
}

OpenCVCameraInputHandler::OpenCVCameraInputHandler(int cam1, int cam2) : m_cam1(cam1), m_cam2(cam2)
{
	flip_status_cam[0] = NOFLIP;
	flip_status_cam[1] = BOTH;
}

bool OpenCVCameraInputHandler::openCams(int cam1,int cam2)
{
  std::cout << "OpenCVCameraInputHandler: openCams(int cam1,int cam2)" << std::endl;
  m_cam1 = cam1;
  m_cam2 = cam2;
  return openCams();
}

bool OpenCVCameraInputHandler::openCams()
{
  std::cout << "OpenCVCameraInputHandler: openCams()" << std::endl;
  m_cap1.open(m_cam1);
  m_cap2.open(m_cam2);
  if(!m_cap1.isOpened() || !m_cap2.isOpened())
  {
    std::cout << "Error: Cameras could not be opened. Aborting" << std::endl;
    m_cap1.release();
    m_cap2.release();
    return false;
  }

  double l_fps = m_cap1.get(CV_CAP_PROP_FPS);
  double r_fps = m_cap2.get(CV_CAP_PROP_FPS);
  int l_h = (int)m_cap1.get(CV_CAP_PROP_FRAME_HEIGHT);
  int r_h = (int)m_cap2.get(CV_CAP_PROP_FRAME_HEIGHT);
  int l_w = (int)m_cap1.get(CV_CAP_PROP_FRAME_WIDTH);
  int r_w = (int)m_cap2.get(CV_CAP_PROP_FRAME_WIDTH);
//    std::cout << "   l      r " << std::endl;
//    std::cout << "fps " << l_fps << "  " << r_fps << std::endl;
//  std::cout << "size " << l_w << " x " << l_h << " | " << r_w << " x " << r_h << std::endl;
  if(l_h != r_h || l_w != r_w)
  {
    std::cout << "Warning: Camera images do not have same resolution." << std::endl;
    std::cout << "Warning: resolutions are: l: w x h | r: w x h " << std::endl;
    std::cout << "Warning: l: "<<l_w << " x " << l_h << " | r: " << r_w << " x " << r_h << std::endl;
  }

  return true;
}

OpenCVCameraInputHandler::~OpenCVCameraInputHandler()
{
  //dtor
}

bool OpenCVCameraInputHandler::grabFrames()
{
  return ( m_cap1.grab() & m_cap2.grab() );
}

void OpenCVCameraInputHandler::retrieveFrame(cv::Mat& frame, int cam)
{
  if(m_cam1 == cam)
    m_cap1.retrieve(frame);
  else if(m_cam2 == cam)
    m_cap2.retrieve(frame);
  else
    std::cout << "No cam with device ID: " << cam << std::endl;

  if(flip_status_cam[cam - 1] != NOFLIP)
    flip(frame,frame,flip_status_cam[cam - 1]);

  if(frame.empty())
    std::cout << "retrieved empty frame" << std::endl;
}

void OpenCVCameraInputHandler::retrieveFrames(cv::Mat& frame1, cv::Mat& frame2, int cam1, int cam2)
{
  retrieveFrame(frame1,cam1);
  retrieveFrame(frame2,cam2);
}

void OpenCVCameraInputHandler::readFrame(cv::Mat& frame, int cam)
{
  if(m_cam1 == cam)
    m_cap1.read(frame);
  else if(m_cam2 == cam)
    m_cap2.read(frame);
  else
    std::cout << "No such cam with device ID: " << cam << std::endl;

  if(frame.empty())
    std::cout << "read empty frame" << std::endl;
}
