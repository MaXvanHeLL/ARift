#include "../include/CameraInputHandler.h"
#include "../include/IDSuEyeInputHandler.h"
#include "../include/ARiftControl.h"
#include "../include/Helpers.h"

#include <opencv2/core/core.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <iostream>

#define GetCurrentDir _getcwd

using namespace cv;

ARiftControl::ARiftControl()
{
  //ctor [Debug]
	cameraBufferLeft_ = new unsigned char[CAMERA_BUFFER_LENGTH];
	cameraBufferRight_ = new char[CAMERA_BUFFER_LENGTH];
	cameraMutexLeft_ = CreateMutex(NULL, FALSE, L"Camera Left Mutex");

	if (cameraMutexLeft_ == NULL)
	{
		std::cout << "Create Mutex error!" << std::endl;
	}

	String picture1_string = "data/test_picture1.jpg";
	String picture2_string = "data/picture_2.bmp";

	picture_1_ = imread(picture1_string, CV_LOAD_IMAGE_COLOR);   // Read the file
	picture_2_ = imread(picture2_string, CV_LOAD_IMAGE_COLOR);

	if (!picture_1_.data)	// Check for invalid input
	{
		std::cout << "Could not open or find the image: " << picture1_string << std::endl;
	}
	if (!picture_2_.data)	// Check for invalid input
	{
		std::cout << "Could not open or find the image: " << picture2_string << std::endl;
	}
}

ARiftControl::~ARiftControl()
{
  //dtor
  if(cam_input != NULL) delete cam_input;
}

void ARiftControl::init()
{
  char cCurrentPath[FILENAME_MAX];

  if (GetCurrentDir(cCurrentPath, sizeof(cCurrentPath)))
  {
    cCurrentPath[sizeof(cCurrentPath) - 1] = '\0';
    std::cout << "The current working directory is " <<  cCurrentPath << std::endl << std::endl;
  } else {
    std::cout << "Error retrieving working path." << std::endl << std::endl;
  }
  base_save_name = getTimeString();
  cam_input = new IDSuEyeInputHandler();
  cam_input->openCams(CAM1,CAM2);
  cam_input->readFrame(left_pic,CAM1);
  cam_input->readFrame(right_pic,CAM2);
  if(left_pic.empty() || right_pic.empty())
    std::cout << "Warning empty image(s) "<< std::endl;

  full_view = Mat::zeros(Size(RIFT_RESOLUTION_WIDTH,RIFT_RESOLUTION_HEIGHT),left_pic.type());
  full_view_undist = Mat::zeros(Size(RIFT_RESOLUTION_WIDTH,RIFT_RESOLUTION_HEIGHT),left_pic.type());

  left_camera_mat = Mat::zeros(3,3,CV_64FC1);
  right_camera_mat = Mat::zeros(3,3,CV_64FC1);

  left_camera_mat.at<double>(0,0) = 429; // fx
  left_camera_mat.at<double>(1,1) = 431; // fy
  left_camera_mat.at<double>(2,2) = 1;
  left_camera_mat.at<double>(0,2) = 359; // cx
  left_camera_mat.at<double>(1,2) = 209; // cy
  left_distortion = (Mat_<double>(4,1) << -0.305 , 0.094, 0.001, -0.0022 ); //kc

  right_camera_mat.at<double>(0,0) = 430; // fx
  right_camera_mat.at<double>(1,1) = 433; // fy
  right_camera_mat.at<double>(2,2) = 1;
  right_camera_mat.at<double>(0,2) = 399; // cx
  right_camera_mat.at<double>(1,2) = 233; // cy
  right_distortion = (Mat_<double>(4,1) <<  -0.291, 0.076, 0.0, -0.0024 ); //kc
  std::cout << "Computing undistortion maps " << std::endl;
  initUndistortRectifyMap(left_camera_mat, left_distortion, Mat(),
      getOptimalNewCameraMatrix(left_camera_mat, left_distortion, left_pic.size(), 1, left_pic.size(), 0),
      left_pic.size(), CV_16SC2, left_map1, left_map2);

  initUndistortRectifyMap(right_camera_mat, right_distortion, Mat(),
      getOptimalNewCameraMatrix(right_camera_mat, right_distortion, right_pic.size(), 1, right_pic.size(), 0),
      right_pic.size(), CV_16SC2, right_map1, right_map2);

}

bool ARiftControl::getImages()
{

  cam_input->grabFrames();
//    cam_input->retrieveFrames(left_pic, right_pic, CAM1, CAM2);
	WaitForSingleObject(cameraMutexLeft_, INFINITE);
  cam_input->retrieveFrame(left_pic, CAM1, cameraBufferLeft_);
	ReleaseMutex(cameraMutexLeft_);

  cam_input->retrieveFrame(right_pic, CAM2, cameraBufferLeft_);
  if(left_pic.empty() || right_pic.empty())
  {
    std::cout << "Warning empty image(s) in loop "<< std::endl;
    return false;
  }
  return true;
}

void ARiftControl::createDisplay()
{
  resize(left_pic, left_resized,
             Size(RIFT_RESOLUTION_WIDTH/2,RIFT_RESOLUTION_HEIGHT),0,0,INTER_LINEAR);
  resize(right_pic,right_resized,
             Size(RIFT_RESOLUTION_WIDTH/2,RIFT_RESOLUTION_HEIGHT),0,0,INTER_LINEAR);

  left_resized.copyTo(full_view(Rect(0,0,RIFT_RESOLUTION_WIDTH/2,RIFT_RESOLUTION_HEIGHT)));
  right_resized.copyTo(full_view(Rect(RIFT_RESOLUTION_WIDTH/2,0,RIFT_RESOLUTION_WIDTH/2,RIFT_RESOLUTION_HEIGHT)));
}

void ARiftControl::undistortImages()
{
  remap(left_pic, left_undistorted, left_map1, left_map2, INTER_LINEAR);
  remap(right_pic, right_undistorted, right_map1, right_map2, INTER_LINEAR);

  resize(left_undistorted, left_resized,
             Size(RIFT_RESOLUTION_WIDTH/2,RIFT_RESOLUTION_HEIGHT),0,0,INTER_LINEAR);
  resize(right_undistorted,right_resized,
             Size(RIFT_RESOLUTION_WIDTH/2,RIFT_RESOLUTION_HEIGHT),0,0,INTER_LINEAR);

  left_resized.copyTo(full_view_undist(Rect(0,0,RIFT_RESOLUTION_WIDTH/2,RIFT_RESOLUTION_HEIGHT)));
  right_resized.copyTo(full_view_undist(Rect(RIFT_RESOLUTION_WIDTH/2,0,RIFT_RESOLUTION_WIDTH/2,RIFT_RESOLUTION_HEIGHT)));
}

void ARiftControl::handleKey(char key)
{
  switch (key)
  {
    case 'q': // quit
    case 'Q': // quit
    case 27:  // quit by escape key
      {
        running = false;
        break;
      }
    case 'f': // flip
      {
        hanldeFlip();
        break;
      }
    case 's': // save
      {
        handleSave();
        break;
      }
    case 'a':
      {
        handleCameraAutoFeatures();
        break;
      }
    default:
      break;
  }
}

void ARiftControl::hanldeFlip()
{
  putText(full_view,"CHOOSE IMAGE TO FLIP l/r",Point(0,RIFT_RESOLUTION_HEIGHT/2),FONT_HERSHEY_SIMPLEX,1,Scalar(0,255,0),2);
  putText(full_view,"CHOOSE IMAGE TO FLIP l/r",Point(RIFT_RESOLUTION_WIDTH/2,RIFT_RESOLUTION_HEIGHT/2),FONT_HERSHEY_SIMPLEX,1,Scalar(0,255,0),2);
  imshow("both",full_view);
  char key_2 = waitKey(0);
  if(key_2 == 'l')
    if(cam_input->flip_status_cam[0] == NOFLIP)
      cam_input->flip_status_cam[0] = HORIZONTAL;
    else
      cam_input->flip_status_cam[0] = (FlipStatus)(cam_input->flip_status_cam[0] - 1);
  else if(key_2 == 'r')
    if(cam_input->flip_status_cam[1] == NOFLIP)
      cam_input->flip_status_cam[1] = HORIZONTAL;
    else
      cam_input->flip_status_cam[1] = (FlipStatus)(cam_input->flip_status_cam[1] - 1);

  std::cout << " Flip status l: " << cam_input->flip_status_cam[0];
  std::cout << ", r:" << cam_input->flip_status_cam[1] << std::endl;
}

void ARiftControl::handleSave()
{
  std::stringstream save_name_left;
  std::stringstream save_name_right;
  save_name_left << base_save_name << "_left_" << write_counter << ".bmp";
  save_name_right << base_save_name << "_right_" << write_counter << ".bmp";
  std::cout << "write images ? (press s to confirm)" << std::endl;
  std::cout << " " <<save_name_left.str() << std::endl <<" " <<  save_name_right.str() << std::endl << std::endl;
  char key_2 = waitKey(0);
  if(key_2 == 's')
  {
    std::cout << "saving images ";
    imwrite(save_name_left.str(),left_pic);
    imwrite(save_name_right.str(),right_pic);
    write_counter++;
    std::cout << "done" << std::endl;
  }
}

void ARiftControl::handleCameraAutoFeatures()
{
  std::stringstream text;
  text << "choose cam";
  Mat view_cpy = full_view.clone();
  putText(view_cpy,"TEST STRING",Point(0,25),FONT_HERSHEY_SIMPLEX,0.5,Scalar(0,255,0),1);
  putText(view_cpy,"TEST STRING",Point(RIFT_RESOLUTION_WIDTH/2,30),FONT_HERSHEY_SIMPLEX,0.5,Scalar(0,255,0),1);
  putText(view_cpy,text.str(),Point(0,RIFT_RESOLUTION_HEIGHT/2),FONT_HERSHEY_SIMPLEX,1,Scalar(0,255,0),2);
  putText(view_cpy,text.str(),Point(RIFT_RESOLUTION_WIDTH/2,RIFT_RESOLUTION_HEIGHT/2),FONT_HERSHEY_SIMPLEX,1,Scalar(0,255,0),2);
  imshow("both",view_cpy);

  char key_2 = waitKey(0);
  bool auto_feature = false;
  int cam = CAM1;

  std::stringstream text2;
  text2 << "choose auto feature to switch: Gain, Shutter";
  view_cpy = full_view.clone();
  if(key_2 == 'l')
  {
    cam = CAM1;
    putText(view_cpy,text2.str(),Point(0,RIFT_RESOLUTION_HEIGHT/2),FONT_HERSHEY_SIMPLEX,0.5,Scalar(0,255,0),1);
  }
  else if(key_2 == 'r')
  {
    cam = CAM2;
    putText(view_cpy,text2.str(),Point(RIFT_RESOLUTION_WIDTH/2,RIFT_RESOLUTION_HEIGHT/2),FONT_HERSHEY_SIMPLEX,0.5,Scalar(0,255,0),1);
  }
  imshow("both",view_cpy);
  char key_3 = waitKey(0);
  if(key_3 == 'g')
  {
    auto_feature = ((IDSuEyeInputHandler*)cam_input)->switchAutoSensorGain(cam);
    std::cout << "Auto gain on cam " << key_2 << " is " << (auto_feature ? "ON" : "OFF") << std::endl;
  }
  else if (key_3 == 's')
  {
    auto_feature = ((IDSuEyeInputHandler*)cam_input)->switchAutoSensorShutter(cam);
    std::cout << "Auto shutter on cam " << key_2 << " is " << (auto_feature ? "ON" : "OFF") << std::endl;
  }
}
