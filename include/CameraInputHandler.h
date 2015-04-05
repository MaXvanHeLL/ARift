#ifndef CAMERAINPUTHANDLER_H
#define CAMERAINPUTHANDLER_H

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

enum FlipStatus {NOFLIP = -2, BOTH = -1, VERTICAL = 0, HORIZONTAL = 1};

class CameraInputHandler
{
  public:
    CameraInputHandler();
    CameraInputHandler(int cam1, int cam2);
    virtual ~CameraInputHandler();

    bool openCams(int cam1,int cam2);
    virtual bool openCams();

    virtual bool grabFrames();
    virtual void retrieveFrame(cv::Mat& frame, int cam);
    void retrieveFrames(cv::Mat& frame1, cv::Mat& frame2, int cam1, int cam2);

    virtual void readFrame(cv::Mat& frame, int cam);

	  FlipStatus flip_status_cam[2];

    cv::VideoCapture m_cap1;
    cv::VideoCapture m_cap2;
  protected:
    // device ID of the cameras
    int m_cam1;
    int m_cam2;


    // last frame grabbed frames from respective cameras
    cv::Mat m_left_frame;
    cv::Mat m_right_frame;
};

#endif // INPUTHANDLER_H
