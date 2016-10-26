
// ARift includes
#include "include/ARiftControl.h"
#include "include/OculusHMD.h"
#include "include/GraphicsAPI.h"
#include "include\Helpers.h"
#include <iostream>
#include <string>
#include <windows.h>
#include <chrono>
#include <thread>

// LSD-SLAM library stuff
#include <highgui.h>
#include <opencv2\opencv.hpp>
#include <cv.h>
#include "util/global_funcs.h"
#include "util/Undistorter.h"
#include "util/settings.h"
#include "slam_system.h"
#include "live_slam_wrapper.h"
// #include "io_wrapper/OpenCVImageStreamThread.h"
#include "io_wrapper/IDSuEyeCameraStreamThread.h"
#include "include/LsdSlam3D.h"


using namespace std;
// ********************************************************************************
 DWORD WINAPI directXHandling(LPVOID lpArg);
 void render(ARiftControl* arift_c);
 LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

#define SHOW_FPS false
#define LsdSlam_CAM 2 // using left or right camera for lsd-slam tracking and mapping
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 2000.0f;
const float SCREEN_NEAR = 0.1f;
GraphicsAPI* dx11 = NULL;
// ********************************************************************************

char key;
int main(int, char**)
{
	// DirectX Graphics and OculusHMD --------------------------------------------------------------------
	dx11 = new GraphicsAPI();
  HANDLE handle_render_thread = 0;
  ARiftControl cont;

  if (AR_HMD_ENABLED)
  {
    cont.init(dx11);
    std::cout << "[Graphics] DirectX init done" << std::endl;
  }  

	//  Activate the Graphics (DirectX11) Thread
    handle_render_thread = CreateThread(NULL, 0,
	   directXHandling, &cont, 0, NULL);
  
  std::cout << "Starting CameraStream" << std::endl;
  cont.start();

	if (LsdSlam_UNDISTORTION) // if shader undistortion is enabled, LsdSlam needs to wait until the first undistorted shader img is ready
	{
		while (true)
		{
			if (dx11->isUndistortionReady())
				break;
		}
	}
	else // LsdSlam Thread needs to wait until autoshutter of cameras is finished to get a bright origin image
	{
		unsigned int autoShutter_ready = 1000;
		std::this_thread::sleep_for(std::chrono::milliseconds(autoShutter_ready));
	}


	// Activate LSD-SLAM --------------------------------------------------------------------------------
	std::cout << "[CV] Init LSD-SLAM Tracking and Mapping" << std::endl;
  // std::string calib_fn = std::string(LsdSlam_DIR) + "/data/out_camera_data.xml"; // load calibration
  std::string calib_fn = std::string(LsdSlam_DIR) + "/data/rift_camera_data.xml"; // load calibration

	CvCapture* capture = cvCaptureFromCAM(LsdSlam_CAM); // Capture using any camera connected to your system
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, 640);
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, 480);

	// lsd_slam::OpenCVImageStreamThread* inputStream = new lsd_slam::OpenCVImageStreamThread();
	lsd_slam::IDSuEyeCameraStreamThread* inputStream = new lsd_slam::IDSuEyeCameraStreamThread();
	inputStream->setCalibration(calib_fn);
	inputStream->setCameraCapture(capture);
	if (LsdSlam_UNDISTORTION)
		inputStream->setIDSuEyeCameraStream(dx11->undistortedShaderBuffer_, dx11->undistortedShaderMutex_, true);
	else
		inputStream->setIDSuEyeCameraStream(cont.camInput_->lsdslamBuffer_, cont.camInput_->lsdslamMutex_, false);
	inputStream->run();

	// Init LSD-SLAM with outputstream and inputstream
	lsd_slam::Output3DWrapper* outputStream = new lsd_slam::LsdSlam3D(inputStream->width(), inputStream->height());
	lsd_slam::LiveSLAMWrapper slamNode(inputStream, outputStream);
	dx11->setLsdSlamTrackingAndMapping(outputStream);

	// -- [Show Undistortion | AR Oculus Rift CAM
	//unsigned char* buffer = new unsigned char[CAMERA_BUFFER_LENGTH];
	//WaitForSingleObject(inputStream->cameraMutex_, INFINITE);
	//memcpy(buffer, inputStream->cameraBuffer_, CAMERA_BUFFER_LENGTH);
	//ReleaseMutex(inputStream->cameraMutex_);
	//IplImage* frame = cvCreateImageHeader(cvSize(CAMERA_WIDTH, CAMERA_HEIGHT), IPL_DEPTH_8U, 4);
	//frame->imageData = (char*)buffer;
	//frame->imageDataOrigin = frame->imageData;
	//IplImage* resized_frame = inputStream->ResizeFrame(frame, LsdSlam_CAM_WIDTH, LsdSlam_CAM_HEIGHT);
	//printf("wh(%d, %d)\n", resized_frame->width, resized_frame->height);
	//cv::Mat mymat = cv::Mat(resized_frame, true);
	//cv::Mat tracker_display = cv::Mat::ones(640, 480, CV_8UC3);
	//cv::circle(mymat, cv::Point(100, 100), 20, cv::Scalar(255, 1, 0), 5);
	//cv::imshow("Camera_Output_Undist", mymat);
	//std::cout << "Tracking and Mapping Camera in 3D..." << std::endl;
	//if (buffer != nullptr)
	//	delete buffer;

	// start LSD-SLAM Thread
	slamNode.Loop();

	if (inputStream != nullptr)
		delete inputStream;
	if (outputStream != nullptr)
		delete outputStream;

	cvReleaseCapture(&capture); //Release capture.

  return 0;
}

// Multithreaded Version
DWORD WINAPI directXHandling(LPVOID lpArg)
{
	ARiftControl* arift_c = (ARiftControl*)lpArg;

	if (AR_HMD_ENABLED)
	{
		// install the Oculus Rift and GraphicsAPI Renderer and init Render Thread
		OculusHMD::initialization(dx11);
		OculusHMD::instance()->calculateFOV();
	}

	// clear out the window class for use
	ZeroMemory(&dx11->window_class_, sizeof(WNDCLASSEX));

	// fill in the struct with the needed information
	dx11->window_class_.cbSize = sizeof(WNDCLASSEX);
	dx11->window_class_.style = CS_HREDRAW | CS_VREDRAW;
	dx11->window_class_.lpfnWndProc = WindowProc;
	dx11->window_class_.hInstance = dx11->hinstance_;
	dx11->window_class_.hCursor = LoadCursor(NULL, IDC_ARROW);
	dx11->window_class_.hbrBackground = (HBRUSH)COLOR_WINDOW;
	dx11->window_class_.lpszClassName = dx11->applicationName_;

	// register the window class
	RegisterClassEx(&dx11->window_class_);

	// [Testing Purpose:] use 960 x 600 windowed for oculus view
	// create the window and use the result as the handle
	dx11->window_ = CreateWindowEx(NULL,
		dx11->applicationName_,    // name of the window class
		L"DirectX Render Scene",   // title of the window
		WS_OVERLAPPEDWINDOW,       // window style
		0,                         // x-position of the window
		0,                         // y-position of the window
		RIFT_RESOLUTION_WIDTH,     // width of the window
		RIFT_RESOLUTION_HEIGHT,    // height of the window
		NULL,                      // we have no parent window, NULL
		NULL,                      // we aren't using menus, NULL
		dx11->hinstance_,          // application handle
		NULL);                     // used with multiple windows, NULL

	dx11->InitD3D(RIFT_RESOLUTION_WIDTH, RIFT_RESOLUTION_HEIGHT, VSYNC_ENABLED, dx11->window_, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR, arift_c);
	ShowWindow(dx11->window_, SW_SHOW); 	// display the window on the screen
	SetFocus(dx11->window_); // set window priority

	if (AR_HMD_ENABLED)
		OculusHMD::instance()->configureStereoRendering();		

	// Reading Videocard Information and writing to a File
	ofstream myfile;
	char videocard[128];
	int videomemory;
	myfile.open("VideocardConfig.txt");
	dx11->GetVideoCardInfo(videocard, videomemory);
	myfile << videocard << endl;
	myfile << videomemory << endl;
	myfile.close();

	MSG msg;
	bool frame_return;

	// wait for the next message in the queue, store the result in 'msg'
	// Enter the infinite message loop
  std::string past;
  unsigned int frame_count = 0;
  int fps = 0;
	while (TRUE)
	{
		// Check to see if any messages are waiting in the queue
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// translate keystroke messages into the right format
			TranslateMessage(&msg);

			// send the message to the WindowProc function
			DispatchMessage(&msg);
		}
		// If the message is WM_QUIT, exit the while loop
		if (msg.message == WM_QUIT)
			break;

    if (msg.message == WM_CHAR)
    {
      //std::cout << "recieved message WM_CHAR : " << msg.wParam << std::endl;
      arift_c->handleKey((char)msg.wParam);
    }
		// Run "game" code here
    // get fps
    std::string now = getTimeString("%S");
    if (SHOW_FPS  && past.compare(now) != 0)
    {
      std::cout << "Time: " << getTimeString() << " fps: " << fps << " frame count:" << frame_count << std::endl;
      fps = 0;
    }
    past = now;
    frame_count++;
    fps++;

    arift_c->camInput_->grabFrames();
		frame_return = dx11->Frame();
    
	}
  delete dx11;
  arift_c->stop();
  if (AR_HMD_ENABLED)
    delete OculusHMD::instance();
	// return this part of the WM_QUIT message to Windows
	return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// sort through and find what code to run for the message given
	switch (message)
	{
    case WM_CHAR:
    {
      if (char(wParam) != 27)
        break;
    }
		// this message is read when the window is closed
		case WM_DESTROY:
		{
			// close the application entirely
			PostQuitMessage(0);
			return 0;
		} break;
	}
	// Handle any messages the switch statement didn't
	return DefWindowProc(hWnd, message, wParam, lParam);
}

