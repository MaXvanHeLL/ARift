
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include "include/ARiftControl.h"
#include <iostream>
using namespace cv;
// Hallo VS!?
int main(int, char**)
{
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
  while(cont.keepRunning())
  {

    if(cont.getImages())
    {
      cont.createDisplay();
      imshow("both",cont.full_view);

      cont.undistortImages();
      imshow("undist",cont.full_view_undist);

    }
    // main control loop
    char key = waitKey(20);
    cont.handleKey(key);
  }
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
