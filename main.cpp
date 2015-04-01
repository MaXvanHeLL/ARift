// OpenCV includes
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
// Include the OculusVR SDK
// #define OVR_D3D_VERSION 11
// #include "OVR_CAPI.h"
// #include "OVR_CAPI_D3D.h"
// ARift includes
#include "include/ARiftControl.h"
#include "include/OculusHMD.h"
#include "include/GraphicsAPI.h"
#include <iostream>
#include <windows.h>

#define AR_HMD_ENABLED 1

using namespace cv;
using namespace std;
// ********************************************************************************
DWORD WINAPI directXHandling(LPVOID lpArg);
void render(ARiftControl* arift_c);
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// not used currently
const bool FULL_SCREEN = true;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;
GraphicsAPI* dx11 = NULL;
// ********************************************************************************

int main(int, char**)
{
	dx11 = new GraphicsAPI();
  HANDLE handle_render_thread = 0;
  ARiftControl cont;

  if (AR_HMD_ENABLED)
  {
	  cont.init();
    std::cout << "init done" << std::endl;
  }  

	// start the Render Thread
  handle_render_thread = CreateThread(NULL, 0,
	  directXHandling, &cont, 0, NULL);

	// namedWindow("camera_freeze", 1);
  // namedWindow("undist",1);
  // namedWindow("both", CV_WINDOW_FULLSCREEN);
  // cvSetWindowProperty("both", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
//    cvNamedWindow("both", CV_WINDOW_NORMAL);
//    cvSetWindowProperty("both", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
  // waitKey(0);
  
  std::cout << "Starting Camera loop" << std::endl;
  cont.start();
  while(cont.keepRunning())
  {
		if (AR_HMD_ENABLED)
		{
			// motion tracking debug tests here
			// *****************************************************************
			// float test1; float test2; float test3;
		  // OculusHMD::instance()->trackMotion(test1, test2, test3);
			// *****************************************************************

			if (cont.getImages())
			{
				cont.createDisplay();
				// imshow("both", cont.full_view);

				cont.undistortImages();
				// imshow("undist", cont.full_view_undist);

				 // cv::Mat camera_mat = cv::Mat(CAMERA_HEIGHT, CAMERA_WIDTH, CV_8UC4, cont.cameraBufferLeft_);
				 // imshow("camera_freeze", camera_mat);
			}
			// main control loop
			char key = waitKey(20);
			cont.handleKey(key);
		}
	}
	if (AR_HMD_ENABLED)
		delete OculusHMD::instance();

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
		WS_OVERLAPPEDWINDOW,    // window style
		0,    // x-position of the window
		0,    // y-position of the window
		OculusHMD::instance()->resolution_.w,  // width of the window
		OculusHMD::instance()->resolution_.h,    // height of the window
		NULL,    // we have no parent window, NULL
		NULL,    // we aren't using menus, NULL
		dx11->hinstance_,    // application handle
		NULL);    // used with multiple windows, NULL

	dx11->InitD3D(OculusHMD::instance()->resolution_.w, OculusHMD::instance()->resolution_.h, VSYNC_ENABLED, dx11->window_, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR, arift_c);
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

		// Run "game" code here
		frame_return = dx11->Frame();
	}
	// return this part of the WM_QUIT message to Windows
	return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// sort through and find what code to run for the message given
	switch (message)
	{
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

