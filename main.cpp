
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include "include/ARiftControl.h"
#include "include/OculusHMD.h"
#include <iostream>
#include "DirectXRendering.cpp"

using namespace cv;
// Hallo VS!?
int main(int, char**)
{
  ARiftControl cont;
  cont.init();

  // install the Oculus Rift
  // *****************************************************************
  InitD3D();
  OculusHMD::initialization(); // OculusHMD is a singleton for accessing the Oculus Device in a static way for better comfort
  // *****************************************************************

  namedWindow("undist",1);
  namedWindow("both",1);
  cvSetWindowProperty("both", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
//    cvNamedWindow("both", CV_WINDOW_NORMAL);
//    cvSetWindowProperty("both", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
  waitKey(0);

  std::cout << "Starting main loop" << std::endl;
  cont.start();

  while(cont.keepRunning())
  {
	// motion tracking debug tests here
	// *****************************************************************
	float test1; float test2; float test3;
	OculusHMD::instance()->trackMotion(test1, test2, test3);
	// *****************************************************************

    if(cont.getImages())
    {
      cont.createDisplay();
      imshow("both",cont.full_view);

      cont.undistortImages();
      imshow("undist",cont.full_view_undist);

	  // *****************************************************************
	  OculusHMD::instance()->render(cont.left_undistorted, cont.right_undistorted);
	  // *****************************************************************
    }
    // main control loop
    char key = waitKey(20);
    cont.handleKey(key);
  }
  // *****************************************************************
  delete OculusHMD::instance();
  CleanD3D();
  // *****************************************************************
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
  cv::Point2f pt(len/2., len/2.);
  cv::Mat r = cv::getRotationMatrix2D(pt, angle, 1.0);

  cv::warpAffine(src, dst, r, cv::Size(len, len));
}

