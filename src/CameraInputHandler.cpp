#include "../include/CameraInputHandler.h"
#include <iostream>

CameraInputHandler::CameraInputHandler() : m_cam1(0),m_cam2(1)
{
	flip_status_cam[0] = NOFLIP;
	flip_status_cam[0] = BOTH;
}

CameraInputHandler::CameraInputHandler(int cam1, int cam2) : m_cam1(cam1), m_cam2(cam2)
{
	flip_status_cam[0] = NOFLIP;
	flip_status_cam[0] = BOTH;
}

bool CameraInputHandler::openCams(int cam1,int cam2)
{
  std::cout << "CameraInputHandler: openCams(int cam1,int cam2)" << std::endl;
  m_cam1 = cam1;
  m_cam2 = cam2;
  return openCams();
}

bool CameraInputHandler::openCams()
{
  std::cout << "CameraInputHandler: openCams()" << std::endl;
  m_cap1.open(m_cam1);
  m_cap2.open(m_cam2);
  if(!m_cap1.isOpened() || !m_cap2.isOpened())
  {
    std::cout << "Error: Cameras could not be opened. Aborting" << std::endl;
    m_cap1.release();
    m_cap2.release();
    return false;
  }

  int l_fps = m_cap1.get(CV_CAP_PROP_FPS);
  int r_fps = m_cap2.get(CV_CAP_PROP_FPS);
  int l_h = m_cap1.get(CV_CAP_PROP_FRAME_HEIGHT);
  int r_h = m_cap2.get(CV_CAP_PROP_FRAME_HEIGHT);
  int l_w = m_cap1.get(CV_CAP_PROP_FRAME_WIDTH);
  int r_w = m_cap2.get(CV_CAP_PROP_FRAME_WIDTH);
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

CameraInputHandler::~CameraInputHandler()
{
  //dtor
}

bool CameraInputHandler::grabFrames()
{
  return ( m_cap1.grab() & m_cap2.grab() );
}

void CameraInputHandler::retrieveFrame(cv::Mat& frame, int cam)
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

void CameraInputHandler::retrieveFrames(cv::Mat& frame1, cv::Mat& frame2, int cam1, int cam2)
{
  retrieveFrame(frame1,cam1);
  retrieveFrame(frame2,cam2);
}

void CameraInputHandler::readFrame(cv::Mat& frame, int cam)
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
