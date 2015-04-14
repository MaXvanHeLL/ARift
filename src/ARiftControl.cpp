#include "../include/IDSuEyeInputHandler.h"
#include "../include/ARiftControl.h"
#include "../include/Helpers.h"

#include <opencv2/core/core.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

#define GetCurrentDir _getcwd

using namespace cv;

ARiftControl::ARiftControl()
{
  // Write read in from file for this
  left_cam_params_.Nxc = -39.0f;
  left_cam_params_.Nyc = 90.0f; 
  left_cam_params_.z = -250.0f;
  left_cam_params_.p6 = 0.0f;
  left_cam_params_.p5 = 16.264f;
  left_cam_params_.p4 = 109.7055f;
  left_cam_params_.p3 = 289.2309f;
  left_cam_params_.p2 = 372.8583f;
  left_cam_params_.p1 = 654.9667f;
  left_cam_params_.p0 = 717.4737f;
  left_cam_params_.c = 0.9999f;
  left_cam_params_.d = -0.00019449f;
  left_cam_params_.e = -0.00030843f;
  left_cam_params_.xc = 214.4453f;
  left_cam_params_.yc = 353.3091f;

  right_cam_params_.Nxc = 79.0f; 
  right_cam_params_.Nyc = 94.0f;
  right_cam_params_.z = -250.0f;
  right_cam_params_.p6 = 50.2189f;
  right_cam_params_.p5 = 313.8636f;
  right_cam_params_.p4 = 759.1147f;
  right_cam_params_.p3 = 864.7065f;
  right_cam_params_.p2 = 420.4562f;
  right_cam_params_.p1 = 438.6404f;
  right_cam_params_.p0 = 628.5998f;
  right_cam_params_.c = 0.9993f;
  right_cam_params_.d =  0.000046388f;
  right_cam_params_.e = -0.000052631f;
  right_cam_params_.xc = 238.1835f;
  right_cam_params_.yc = 391.6032f;
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
  cam_input->retrieveFrame(left_pic, CAM1);
  cam_input->retrieveFrame(right_pic, CAM2);
  if(left_pic.empty() || right_pic.empty())
    std::cout << "Warning empty image(s) "<< std::endl;

  full_view = Mat::zeros(Size(RIFT_RESOLUTION_WIDTH,RIFT_RESOLUTION_HEIGHT),left_pic.type());
  full_view_undist = Mat::zeros(Size(RIFT_RESOLUTION_WIDTH,RIFT_RESOLUTION_HEIGHT),left_pic.type());

  left_camera_mat = Mat::zeros(3,3,CV_64FC1);
  right_camera_mat = Mat::zeros(3,3,CV_64FC1);

}

bool ARiftControl::getImages()
{
  cam_input->retrieveFrame(left_pic, CAM1);

  cam_input->retrieveFrame(right_pic, CAM2);
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
  //std::cout << "ARiftControl::handleKey recived char: " << key << std::endl;
  float step = 0.2f;
  switch (key)
  {
    case 27:  // quit by escape key
    {
      //running = false;
      break;
    }
    case 'w':
    {
      left_cam_params_.Nyc += step;
      right_cam_params_.Nyc -= step;
      break;
    }
    case 'a':
    {
      left_cam_params_.Nxc -= step;
      right_cam_params_.Nxc += step;
      break;
    }
    case 's':
    {
      left_cam_params_.Nyc -= step;
      right_cam_params_.Nyc += step;
      break;
    }
    case 'd':
    {
      left_cam_params_.Nxc += step;
      right_cam_params_.Nxc -= step;
      break;
    }

    case 'W':
    {
      left_cam_params_.Nyc += step;
      right_cam_params_.Nyc += step;
      break;
    }
    case 'A':
    {
      left_cam_params_.Nxc -= step;
      right_cam_params_.Nxc -= step;
      break;
    }
    case 'S':
    {
      left_cam_params_.Nyc -= step;
      right_cam_params_.Nyc -= step;
      break;
    }
    case 'D':
    {
      left_cam_params_.Nxc += step;
      right_cam_params_.Nxc += step;
      break;
    }
    case 'p':
    {
      std::cout << "(x, y) left:  (" << left_cam_params_.Nxc  << ", " << left_cam_params_.Nyc << " ) ";
      std::cout << " right: (" << right_cam_params_.Nxc << ", " << right_cam_params_.Nyc << " ) " << std::endl;
      break;
    }
    //case 'f': // flip
    //  {
    //    hanldeFlip();
    //    break;
    //  }
    //case 's': // save
    //  {
    //    handleSave();
    //    break;
    //  }
    //case 'a':
    //  {
    //    handleCameraAutoFeatures();
    //    break;
    //  }
    default:
      break;
  }
}

void ARiftControl::hanldeFlip()
{
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
