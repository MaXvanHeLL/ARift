#include "../include/IDSuEyeInputHandler.h"
#include "../include/ARiftControl.h"
#include "../include/GraphicsAPI.h"
#include "../include/OculusHMD.h"
#include "../include/Helpers.h"


#include <iostream>
#include <math.h>
#define GetCurrentDir _getcwd

using namespace cv;

ARiftControl::ARiftControl()
{
  // Write read in from file for this
  if (HMD_DISTORTION)
  {
    left_cam_params_.Nxc = -112.4f;
    left_cam_params_.Nyc = 67.2f;
    left_cam_params_.z = -177.0f;
  }
  else
  {
    left_cam_params_.Nxc = -39.0f;
    left_cam_params_.Nyc = 90.0f;
    left_cam_params_.z = -250.0f;
  }
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

  if (HMD_DISTORTION)
  {
    right_cam_params_.Nxc = 112.4f;
    right_cam_params_.Nyc = 67.2f;
    right_cam_params_.z = -177.0f;
  }
  else
  {
    right_cam_params_.Nxc = 79.0f;
    right_cam_params_.Nyc = 94.0f;
    right_cam_params_.z = -250.0f;
  }
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
  base_save_name_ = getTimeString();
  cam_input = new IDSuEyeInputHandler();
  cam_input->openCams(CAM1,CAM2);
  cam_input->retrieveFrame(CAM1);
  cam_input->retrieveFrame(CAM2);

}

void ARiftControl::getImages()
{
  cam_input->retrieveFrame(CAM1);
  cam_input->retrieveFrame(CAM2);

}

void ARiftControl::handleKey(char key)
{
  //std::cout << "ARiftControl::handleKey recived char: " << key << std::endl;
  switch (key)
  {
    case 27:  // quit by escape key
    {
      //running = false;
      break;
    }
    case 'v':
    {
      if (HMD_DISTORTION && AR_HMD_ENABLED)
        OculusHMD::instance()->Recenter();
      break;
    }
    case 'm':
    {
      if (last_key_ == 'm') // ignore long keypress and requre last key to be different
        break;
      if (input_mode_ == InputMode::CAMERA)
        input_mode_ = InputMode::DEFAULT;
      else
        input_mode_ = InputMode(input_mode_ + 1);

      std::cout << "Model changing mode is " << input_mode_ << std::endl;
      break;
    }
    case '.':
    {
      if (input_mode_ == InputMode::MODEL)
        graphics_api_->SetNextModelActive();
      break;
    }
    case ',':
    {
      if (input_mode_ == InputMode::MODEL)
        graphics_api_->SetPreviousModelActive();
      break;
    }
    case 'r':
    {
      if (last_key_ == 'r') // ignore long keypress and requre last key to be different
        break;
      if (input_mode_ == InputMode::MODEL)
      {
        model_auto_rotate_ = !model_auto_rotate_;
        std::cout << "model auto rotate is " << model_auto_rotate_ << std::endl;
      }
      break;
    }
    case 't':
    {
      if (last_key_ == 't') // ignore long keypress and requre last key to be different
        break;
      if (input_mode_ == InputMode::MODEL)
      {
        model_auto_translate_ = !model_auto_translate_;
        std::cout << "model auto translate is " << model_auto_translate_ << std::endl;
      }
      break;
    }
    case 'w':
    {
      if (input_mode_ == InputMode::DEFAULT)
      {
        left_cam_params_.Nyc += step_;
        right_cam_params_.Nyc -= step_;
      }
      else if (input_mode_ == InputMode::MODEL)
      {
        model_offset_y_ += step_;
        changed_model_ = true;
      }
      else if (input_mode_ == InputMode::WORLD)
      {
        world_offset_y_ += step_;
      }
      break;
    }
    case 'a':
    {
      if (input_mode_ == InputMode::DEFAULT)
      {
        left_cam_params_.Nxc -= step_;
        right_cam_params_.Nxc += step_;
      }
      else if (input_mode_ == InputMode::MODEL)
      {
        model_offset_x_ -= step_;
        changed_model_ = true;
      }
      else if (input_mode_ == InputMode::WORLD)
      {
        world_offset_x_ -= step_;
      }
      else if (input_mode_ == InputMode::CAMERA)
      {
        camera_offset_x_ -= step_;
      }
      break;
    }
    case 's':
    {
      if (input_mode_ == InputMode::DEFAULT)
      {
        left_cam_params_.Nyc -= step_;
        right_cam_params_.Nyc += step_;
      }
      else if(input_mode_ == InputMode::MODEL)
      {
        model_offset_y_ -= step_;
        changed_model_ = true;
      }
      else if (input_mode_ == InputMode::WORLD)
      {
        world_offset_y_ -= step_;
      }
      break;
    }
    case 'd':
    {
      if (input_mode_ == InputMode::DEFAULT)
      {
        left_cam_params_.Nxc += step_;
        right_cam_params_.Nxc -= step_;
      }
      else if (input_mode_ == InputMode::MODEL)
      {
        model_offset_x_ += step_;
        changed_model_ = true;
      }
      else if (input_mode_ == InputMode::WORLD)
      {
        world_offset_x_ += step_;
      }
      else if (input_mode_ == InputMode::CAMERA)
      {
        camera_offset_x_ += step_;
      }
      break;
    }
    case 'y':
    {
      if (input_mode_ == InputMode::MODEL)
      {
        model_offset_z_ += step_;
        changed_model_ = true;
      }
      else if (input_mode_ == InputMode::WORLD)
      {
        world_offset_z_ += step_;
      }
      else if (input_mode_ == InputMode::CAMERA)
      {
        camera_offset_z_ += step_;
      }
      break;
    }
    case 'x':
    {
      if (input_mode_ == InputMode::MODEL)
      {
        model_offset_z_ -= step_;
        changed_model_ = true;
      }
      else if (input_mode_ == InputMode::WORLD)
      {
        world_offset_z_ -= step_;
      }
      else if (input_mode_ == InputMode::CAMERA)
      {
        camera_offset_z_ -= step_;
      }
      break;
    }
    case 'q':
    {
      if (input_mode_ == InputMode::MODEL)
      {
        model_rotation_ += step_; //float(fmod(double(model_rotation_ + step_), 360.0));
        changed_model_ = true;
      }
      break;
    }
    case 'e':
    {
      if (input_mode_ == InputMode::MODEL)
      {
        model_rotation_ -= step_;
        //if (model_rotation_ < 0.0f)
        //  model_rotation_ = model_rotation_ + 360.0f;
        
        changed_model_ = true;
      }
      break;
    }
    case 'W':
    {
      if (input_mode_ == InputMode::DEFAULT)
      {
        left_cam_params_.Nyc += step_;
        right_cam_params_.Nyc += step_;
      }
      break;
    }
    case 'A':
    {
      if (input_mode_ == InputMode::DEFAULT)
      {
        left_cam_params_.Nxc -= step_;
        right_cam_params_.Nxc -= step_;
      }
      break;
    }
    case 'S':
    {
      if (input_mode_ == InputMode::DEFAULT)
      {
        left_cam_params_.Nyc -= step_;
        right_cam_params_.Nyc -= step_;
      }
      break;
    }
    case 'D':
    {
      if (input_mode_ == InputMode::DEFAULT)
      {
        left_cam_params_.Nxc += step_;
        right_cam_params_.Nxc += step_;
      }
      break;
    }
		case 'Z':
		{
      if (input_mode_ == InputMode::DEFAULT)
      {
        left_cam_params_.z += step_;
        right_cam_params_.z += step_;
      }
			break;
		}
		case 'z':
		{
      if (input_mode_ == InputMode::DEFAULT)
      {
        left_cam_params_.z -= step_;
        right_cam_params_.z -= step_;
      }
			break;
		}
    case '0':
    {
      step_ = 0.01f;
      break;
    }
    case '1':
    {
      step_ = 0.1f;
      break;
    }
    case '2':
    {
      step_ = 0.2f;
      break;
    }
    case '3':
    {
      step_ = 0.5f;
      break;
    }
    case '4':
    {
      step_ = 1.0f;
      break;
    }
    case '5':
    {
      step_ = 5.0f;
      break;
    }
    case 'o':
    {
      std::cout << "undistortion: " << std::endl;
      std::cout << "(x, y, z) left:  (" << left_cam_params_.Nxc << ", " << left_cam_params_.Nyc << ", " << left_cam_params_.z << " ) ";
      std::cout << " right: (" << right_cam_params_.Nxc << ", " << right_cam_params_.Nyc << ", " << right_cam_params_.z << " ) " << std::endl;
      std::cout << "camera offset y: " << camera_offset_y_ << std::endl;
      std::cout << "world offset: " << std::endl;
      std::cout << "(x,y,z): ( " << world_offset_x_ << "," << world_offset_y_ << "," << world_offset_z_ << ")" << std::endl;
      break;
    }
    case 'P':
    {
      cam_input->changeAutoSensorSpeeds(step_);
      break;
    }
    case 'p':
    {
      cam_input->changeAutoSensorSpeeds(-step_);
      break;
    }
    case 'f':
    {
      std::cout << " cam " << CAM1 << " has " << cam_input->getFrameRate(CAM1) << "frames / s  |";
      std::cout << " cam " << CAM2 << " has " << cam_input->getFrameRate(CAM2) << "frames / s" << std::endl;
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
  last_key_ = key;
}
void ARiftControl::ResetModelChange()
{
  model_offset_x_ = 0.0f;
  model_offset_y_ = 0.0f;
  model_offset_z_ = 0.0f;
  model_rotation_ = 0.0f;
  changed_model_ = false;
}
void ARiftControl::hanldeFlip()
{
}

void ARiftControl::handleSave()
{
}

void ARiftControl::handleCameraAutoFeatures()
{
}
