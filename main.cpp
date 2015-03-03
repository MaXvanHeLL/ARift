// OpenCV includes
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
// Include the OculusVR SDK
#include "OVR_CAPI.h"
// ARift includes
#include "include/ARiftControl.h"
// std includes
#include <iostream>
#include <time.h>
using namespace cv;

int main(int, char**)
{
  // Initializes LibOVR, and the Rift
  ovr_Initialize();

  ARiftControl cont;
  cont.init();

  namedWindow("undist",1);
  namedWindow("both",1);
  cvSetWindowProperty("both", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
//    cvNamedWindow("both", CV_WINDOW_NORMAL);
//    cvSetWindowProperty("both", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
  waitKey(0);
  

  std::cout << "Starting main loop" << std::endl;
  cont.start();
  int count = 0;
  while(cont.keepRunning())
  {

    if(cont.getImages())
    {
      cont.createDisplay();
      imshow("both",cont.full_view);

      cont.undistortImages();
      imshow("undist",cont.full_view_undist);
      // Display when images were displayed
      //time(&raw_time);
      //std::string time_str = ctime(&raw_time);
      //std::cout << "Frame done at " << time_str  << " " << count++<< std::endl;

    }
    // main control loop
    char key = waitKey(20);
    cont.handleKey(key);
  }
  // quit LibOVR
  //ovrHmd_Destroy(hmd);
  ovr_Shutdown();
  return 0;
}

/**
 * Rotate an image
 */
void rotate(cv::Mat& src, double angle, cv::Mat& dst)
{
  if(angle < 0.0001 )
  {
    dst = src.clone();
    return;
  }
  int len = max(src.cols, src.rows);
  cv::Point2f pt((float)(len / 2.), (float)(len / 2.));
  cv::Mat r = cv::getRotationMatrix2D(pt, angle, 1.0);

  cv::warpAffine(src, dst, r, cv::Size(len, len));
}
