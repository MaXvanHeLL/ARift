#include "../include/IDSuEyeInputHandler.h"
#include "../include/ARiftControl.h"
#include "../include/GraphicsAPI.h"
#include "../include/OculusHMD.h"
#include "../include/Helpers.h"

#include <chrono>
#include <ctime>
#include <iostream>
#include <math.h>

#define GetCurrentDir _getcwd


ARiftControl::ARiftControl()
{
  // TODO
  // these calibrations needs to be recomputed with ocamcalib
  // currently the vertical distortion is way larger than the horizintal one
  // Write / read from file for this
  if (HMD_DISTORTION)
  {
    leftCameraParameters_.Nxc = -79.4f;
    leftCameraParameters_.Nyc = 67.2f;
    leftCameraParameters_.z = -177.0f;
  }
  else
  {
    leftCameraParameters_.Nxc = -39.0f;
    leftCameraParameters_.Nyc = 90.0f;
    leftCameraParameters_.z = -250.0f;
  }
  leftCameraParameters_.p9 = (float)(0.030943828391667e+04);
  leftCameraParameters_.p8 = (float)(0.282942249820711e+04);
  leftCameraParameters_.p7 = (float)(1.150239951453818e+04);
  leftCameraParameters_.p6 = (float)(2.755836619048961e+04);
  leftCameraParameters_.p5 = (float)(4.338342090721455e+04);
  leftCameraParameters_.p4 = (float)(4.700973643667896e+04);
  leftCameraParameters_.p3 = (float)(3.520272074605963e+04);
  leftCameraParameters_.p2 = (float)(1.746733371991807e+04);
  leftCameraParameters_.p1 = (float)(0.555549279942498e+04);
  leftCameraParameters_.p0 = (float)(0.132980078899012e+04);
  leftCameraParameters_.c = (float)(1.000374008613590);
  leftCameraParameters_.d = (float)(-9.611375125204626e-05);
  leftCameraParameters_.e = (float)(-1.127978103886716e-04);
  leftCameraParameters_.xc = (float)(2.119393300155207e+02);
  leftCameraParameters_.yc = (float)(3.546241749915297e+02);
  leftCameraParameters_.height = 752.0f;
  leftCameraParameters_.width = 480.0f;

  if (HMD_DISTORTION)
  {
    rightCameraParameters_.Nxc = 79.4f;
    rightCameraParameters_.Nyc = 67.2f;
    rightCameraParameters_.z = -177.0f;
  }
  else
  {
    rightCameraParameters_.Nxc = 79.0f;
    rightCameraParameters_.Nyc = 94.0f;
    rightCameraParameters_.z = -250.0f;
  }
  rightCameraParameters_.p9 = 0.0;
  rightCameraParameters_.p8 = 0.0;
  rightCameraParameters_.p7 = 0.0;
  rightCameraParameters_.p6 = 50.2189f;
  rightCameraParameters_.p5 = 313.8636f;
  rightCameraParameters_.p4 = 759.1147f;
  rightCameraParameters_.p3 = 864.7065f;
  rightCameraParameters_.p2 = 420.4562f;
  rightCameraParameters_.p1 = 438.6404f;
  rightCameraParameters_.p0 = 628.5998f;
  rightCameraParameters_.c = 0.9993f;
  rightCameraParameters_.d =  0.000046388f;
  rightCameraParameters_.e = -0.000052631f;
  rightCameraParameters_.xc = 238.1835f;
  rightCameraParameters_.yc = 391.6032f;
  rightCameraParameters_.width = 752.0f;
  rightCameraParameters_.height = 480.0f;

  lastKeyTime = std::chrono::system_clock::now();
  programStartTime = lastKeyTime;
  minimumKeyInputDelay = std::chrono::duration<double>(0.05); // seconds
}

ARiftControl::~ARiftControl()
{
  //dtor
  if(camInput_ != NULL) delete camInput_;
}

void ARiftControl::init(GraphicsAPI* graphicsAPI)
{
  char cCurrentPath[FILENAME_MAX];

  if (GetCurrentDir(cCurrentPath, sizeof(cCurrentPath)))
  {
    cCurrentPath[sizeof(cCurrentPath) - 1] = '\0';
    std::cout << "The current working directory is " <<  cCurrentPath << std::endl << std::endl;
  } else {
    std::cout << "Error retrieving working path." << std::endl << std::endl;
  }
  baseSaveName_ = getTimeString();
  camInput_ = new IDSuEyeInputHandler();
  camInput_->openCams(CAM1,CAM2);
  camInput_->retrieveFrame(CAM1);
  camInput_->retrieveFrame(CAM2);
  this->graphicsAPI_ = graphicsAPI;
}

void ARiftControl::getImages()
{
  camInput_->retrieveFrame(CAM1);
  camInput_->retrieveFrame(CAM2);

}

void ARiftControl::handleKey(char key)
{
  if (graphicsAPI_ == NULL)
  {
    std::cout << "graphicsAPI unknown disable user input" << std::endl;
    return;
  }
  std::chrono::time_point<std::chrono::system_clock> currentTime = std::chrono::system_clock::now();
  std::chrono::duration<double> timeSinceLastKey = currentTime - lastKeyTime;
  std::cout << "time total: " << std::chrono::duration<double>(currentTime - programStartTime).count();
  std::cout << " since: " << timeSinceLastKey.count() << " key " << (int)key << std::endl;
  switch (key)
  {
    case 'm':
    {
      // ignore long/fast repeating keypress
      if (lastKey_ == 'm' && timeSinceLastKey < minimumKeyInputDelay)
        break;
      lastKeyTime = std::chrono::system_clock::now();
      switch (inputMode_)
      {
        case InputMode::DEFAULT:
        {
          inputMode_ = InputMode::MODEL;
          break;
        }
        case InputMode::MODEL:
        {
          // forget new model state if not promted by 'CR' before
          newModelState_ = oldModelState_;
          graphicsAPI_->SetCurrentModelState(oldModelState_);
          inputMode_ = InputMode::WORLD;
          break;
        }
        case InputMode::WORLD:
        {
          inputMode_ = InputMode::CAMERA;
          //break; // TODO check if we need CAMERA input mode at all
        }
        case InputMode::CAMERA:
        {
          inputMode_ = InputMode::DEFAULT;
          break;
        }
        default:
        {
          std::cout << "Unknown input mode " << (int)inputMode_ << " setting to default" << std::endl;
          inputMode_ = InputMode::DEFAULT;
          break;
        }
      }
      std::cout << "Model changing mode is now " << inputMode_ << std::endl;
      break;
    }
    case (8): // DEL
    {
      // revert model state to original
      if (inputMode_ == InputMode::MODEL)
      {
        newModelState_ = oldModelState_;
        graphicsAPI_->SetCurrentModelState(oldModelState_);
      }
    }
    case (13): // CR
    {
      // forget old model state and keep changes
      if (inputMode_ == InputMode::MODEL)
      {
        oldModelState_ = newModelState_;
        graphicsAPI_->SetCurrentModelState(newModelState_);
      }
      break;
    }
    case '.':
    {
      // ignore long/fast repeating keypress
      if (lastKey_ == '.' && timeSinceLastKey < minimumKeyInputDelay)
        break;
      lastKeyTime = std::chrono::system_clock::now();
      if (inputMode_ == InputMode::MODEL)
      {
        // forget new model state if not promted by 'CR' before
        newModelState_ = oldModelState_;
        graphicsAPI_->SetCurrentModelState(oldModelState_);
        // set next model as current model
        graphicsAPI_->SetNextModelActive();
        newModelState_ = graphicsAPI_->GetCurrentModelState();
        oldModelState_ = newModelState_;
      }
      break;
    }
    case ',':
    {
      // ignore long/fast repeating keypress
      if (lastKey_ == ',' && timeSinceLastKey < minimumKeyInputDelay)
        break;
      lastKeyTime = std::chrono::system_clock::now();
      if (inputMode_ == InputMode::MODEL)
      {
        // forget new model state if not promted by 'CR' before
        newModelState_ = oldModelState_;
        graphicsAPI_->SetCurrentModelState(oldModelState_);
        // set previous model as current model
        graphicsAPI_->SetPreviousModelActive();
        newModelState_ = graphicsAPI_->GetCurrentModelState();
        oldModelState_ = newModelState_;
      }
      break;
    }
    case 'r':
    {
      if (inputMode_ == InputMode::DEFAULT)
      {
        OculusHMD::instance()->Recenter();
        std::cout << "recenter on current pose" << std::endl;
      }
      // ignore long/fast repeating keypress
      if (lastKey_ == 'r' && timeSinceLastKey < minimumKeyInputDelay)
        break;
      if (inputMode_ == InputMode::MODEL && lastKey_ != 'r')
      {
        newModelState_.autoRotate_ = newModelState_.autoRotate_;
        std::cout << "model auto rotate is " << newModelState_.autoRotate_ << std::endl;
        lastKeyTime = std::chrono::system_clock::now();
      }
      break;
    }
    case 't':
    {
      //if (lastKey_ == 't') // ignore long keypress and requre last key to be different
      //  break;
      //if (inputMode_ == InputMode::MODEL)
      //{
      //  model_auto_translate_ = !model_auto_translate_;
      //  std::cout << "model auto translate is " << model_auto_translate_ << std::endl;
      //}
      break;
    }
    case 'w':
    {
      switch (inputMode_)
      {
        case InputMode::DEFAULT:
        {
          leftCameraParameters_.Nyc += step_;
          rightCameraParameters_.Nyc -= step_;
          break;
        }
        case InputMode::MODEL:
        {
          newModelState_.positionY_ += step_;
          break;
        }
        case InputMode::WORLD:
        {
          worldOffsetY_ += step_;
          break;
        }
        case InputMode::CAMERA:
        {
          cameraOffsetY_ += step_;
          break;
        }
        default:
        {
          std::cout << "Unknown input mode" << std::endl;
          break;
        }
      }
      break;
    }
    case 'a':
    {
      switch (inputMode_)
      {
        case InputMode::DEFAULT:
        {
          leftCameraParameters_.Nxc -= step_;
          rightCameraParameters_.Nxc += step_;
          break;
        }
        case InputMode::MODEL:
        {
          newModelState_.positionX_ -= step_;
          break;
        }
        case InputMode::WORLD:
        {
          worldOffsetX_ -= step_;
          break;
        }
        case InputMode::CAMERA:
        {
          cameraOffsetX_ -= step_;
          break;
        }
        default:
        {
          std::cout << "Unknown input mode" << std::endl;
          break;
        }
      }
      break;
    }
    case 's':
    {
      switch (inputMode_)
      {
        case InputMode::DEFAULT:
        {
          leftCameraParameters_.Nyc -= step_;
          rightCameraParameters_.Nyc += step_;
          break;
        }
        case InputMode::MODEL:
        {
          newModelState_.positionY_ -= step_;
          break;
        }
        case InputMode::WORLD:
        {
          worldOffsetY_ -= step_;
          break;
        }
        case InputMode::CAMERA:
        {
          cameraOffsetY_ -= step_;
          break;
        }
        default:
        {
          std::cout << "Unknown input mode" << std::endl;
          break;
        }
      }
      break;
    }
    case 'd':
    {
      switch (inputMode_)
      {
        case InputMode::DEFAULT:
        {
          leftCameraParameters_.Nxc += step_;
          rightCameraParameters_.Nxc -= step_;
          break;
        }
        case InputMode::MODEL:
        {
          newModelState_.positionX_ += step_;
          break;
        }
        case InputMode::WORLD:
        {
          worldOffsetX_ += step_;
          break;
        }
        case InputMode::CAMERA:
        {
          cameraOffsetX_ += step_;
          break;
        }
        default:
        {
          std::cout << "Unknown input mode" << std::endl;
          break;
        }
      }
      break;
    }
    case 'q':
    {
      switch (inputMode_)
      {
        case InputMode::DEFAULT:
        {
          leftCameraParameters_.z += step_;
          rightCameraParameters_.z -= step_;
          break;
        }
        case InputMode::MODEL:
        {
          newModelState_.positionZ_ += step_;
          break;
        }
        case InputMode::WORLD:
        {
          worldOffsetZ_ += step_;
          break;
        }
        case InputMode::CAMERA:
        {
          cameraOffsetZ_ += step_;
          break;
        }
        default:
        {
          std::cout << "Unknown input mode" << std::endl;
          break;
        }
      }
      break;
    }
    case 'e':
    {
      switch (inputMode_)
      {
        case InputMode::DEFAULT:
        {
          leftCameraParameters_.z -= step_;
          rightCameraParameters_.z += step_;
          break;
        }
        case InputMode::MODEL:
        {
          newModelState_.positionZ_ -= step_;
          break;
        }
        case InputMode::WORLD:
        {
          worldOffsetZ_ -= step_;
          break;
        }
        case InputMode::CAMERA:
        {
          cameraOffsetZ_ -= step_;
          break;
        }
        default:
        {
          std::cout << "Unknown input mode" << std::endl;
          break;
        }
      }
      break;
    }
    case 'W':
    {
      switch (inputMode_)
      {
        case InputMode::DEFAULT:
        {
          leftCameraParameters_.Nyc += step_;
          rightCameraParameters_.Nyc += step_;
          break;
        }
        case InputMode::MODEL:
        {
          newModelState_.rotationY_ += step_;
          break;
        }
        case InputMode::WORLD:
          break;
        case InputMode::CAMERA:
          break;
        default:
        {
          std::cout << "Unknown input mode" << std::endl;
          break;
        }
      }
      break;
    }
    case 'A':
    {
      switch (inputMode_)
      {
        case InputMode::DEFAULT:
        {
          leftCameraParameters_.Nxc += step_;
          rightCameraParameters_.Nxc += step_;
          break;
        }
        case InputMode::MODEL:
        {
          newModelState_.rotationX_ -= step_;
          break;
        }
        case InputMode::WORLD:
          break;
        case InputMode::CAMERA:
          break;
        default:
        {
          std::cout << "Unknown input mode" << std::endl;
          break;
        }
      }
      break;
    }
    case 'S':
    {
      switch (inputMode_)
      {
        case InputMode::DEFAULT:
        {
          leftCameraParameters_.Nyc -= step_;
          rightCameraParameters_.Nyc -= step_;
          break;
        }
        case InputMode::MODEL:
        {
          newModelState_.rotationY_ -= step_;
          break;
        }
        case InputMode::WORLD:
          break;
        case InputMode::CAMERA:
          break;
        default:
        {
          std::cout << "Unknown input mode" << std::endl;
          break;
        }
      }
      break;
    }
    case 'D':
    {
      switch (inputMode_)
      {
        case InputMode::DEFAULT:
        {
          leftCameraParameters_.Nxc -= step_;
          rightCameraParameters_.Nxc -= step_;
          break;
        }
        case InputMode::MODEL:
        {
          newModelState_.rotationX_ += step_;
          break;
        }
        case InputMode::WORLD:
          break;
        case InputMode::CAMERA:
          break;
        default:
        {
          std::cout << "Unknown input mode" << std::endl;
          break;
        }
      }
      break;
    }
    case 'Q':
    {
      switch (inputMode_)
      {
        case InputMode::DEFAULT:
        {
          leftCameraParameters_.z += step_;
          rightCameraParameters_.z += step_;
          break;
        }
        case InputMode::MODEL:
        {
          newModelState_.rotationZ_ += step_;
          break;
        }
        case InputMode::WORLD:
          break;
        case InputMode::CAMERA:
          break;
        default:
        {
          std::cout << "Unknown input mode" << std::endl;
          break;
        }
      }
      break;
    }
    case 'E':
    {
      switch (inputMode_)
      {
        case InputMode::DEFAULT:
        {
          leftCameraParameters_.z -= step_;
          rightCameraParameters_.z -= step_;
          break;
        }
        case InputMode::MODEL:
        {
          newModelState_.rotationZ_ -= step_;
          break;
        }
        case InputMode::WORLD:
          break;
        case InputMode::CAMERA:
          break;
        default:
        {
          std::cout << "Unknown input mode" << std::endl;
          break;
        }
      }
      break;
    }
    case 'c':
    {
      interPupillaryOffset_ -= step_ * 0.1f;
      break;
    }
    case 'C':
    {
      interPupillaryOffset_ += step_ * 0.1f;
      break;
    }
    case '+':
    {
      // ignore long/fast repeating keypress
      if (lastKey_ == '+' && timeSinceLastKey < minimumKeyInputDelay)
        break;
      step_ *= 10.0f;
      std::cout << "step_ " << step_ << std::endl;
      lastKeyTime = std::chrono::system_clock::now();
      break;
    }
    case '-':
    {
      // ignore long/fast repeating keypress
      if (lastKey_ == '-' && timeSinceLastKey < minimumKeyInputDelay)
        break;
      step_ /= 10.0f;
      std::cout << "step_ " << step_ << std::endl;
      lastKeyTime = std::chrono::system_clock::now();
      break;
    }
    case '1':
    {
      step_ = 1.0f;
      std::cout << "step_ " << step_ << std::endl;
      break;
    }
    case '2':
    {
      step_ = 2.0f;
      std::cout << "step_ " << step_ << std::endl;
      break;
    }
    case '3':
    {
      step_ = 4.0f;
      std::cout << "step_ " << step_ << std::endl;
      break;
    }
    case '4':
    {
      step_ = 6.0f;
      std::cout << "step_ " << step_ << std::endl;
      break;
    }
    case '5':
    {
      step_ = 8.0f;
      std::cout << "step_ " << step_ << std::endl;
      break;
    }
    case '6':
    {
      step_ = 10.0f;
      std::cout << "step_ " << step_ << std::endl;
      break;
    }
    case 'o':
    {
      // ignore long/fast repeating keypress
      if (lastKey_ == 'o' && timeSinceLastKey < minimumKeyInputDelay)
        break;
      std::cout << "(x, y, z) left:  (" << leftCameraParameters_.Nxc << ", " << leftCameraParameters_.Nyc << ", " << leftCameraParameters_.z << " ) ";
      std::cout << " right: (" << rightCameraParameters_.Nxc << ", " << rightCameraParameters_.Nyc << ", " << rightCameraParameters_.z << " ) " << std::endl;
      std::cout << "world translation offset (x,y,z): (" << worldOffsetX_ << ", " << worldOffsetY_ << ", " << worldOffsetZ_ << " ) " << std::endl;
      lastKeyTime = std::chrono::system_clock::now();
      break;
    }
    case 'P':
    {
      camInput_->changeAutoSensorSpeeds(step_);
      break;
    }
    case 'p':
    {
      camInput_->changeAutoSensorSpeeds(-step_);
      break;
    }
    case 'f':
    {
      // ignore long/fast repeating keypress
      if (lastKey_ == 'f' && timeSinceLastKey < minimumKeyInputDelay)
        break;
      std::cout << " cam " << CAM1 << " has " << camInput_->getFrameRate(CAM1) << "frames / s  |";
      std::cout << " cam " << CAM2 << " has " << camInput_->getFrameRate(CAM2) << "frames / s" << std::endl;
      lastKeyTime = std::chrono::system_clock::now();
      break;
    }
    default:
      break;
  }
  lastKey_ = key;
  // write newModtelState so that it will be rendered for the user
  if (inputMode_ == InputMode::MODEL)
    graphicsAPI_->SetCurrentModelState(newModelState_);
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
