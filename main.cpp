
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include "include/ARiftControl.h"
#include "include/OculusHMD.h"
#include "include/DirectX.h"
#include <iostream>
#include <windows.h>

using namespace cv;
using namespace std;

DWORD WINAPI runDirectX(LPVOID lpArg);
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

DirectX* dx11 = NULL;

int main(int, char**)
{
  ARiftControl cont;
  cont.init();

  // install the Oculus Rift and DirectX Renderer
  // *****************************************************************
  dx11 = new DirectX();
  HANDLE handle_render_thread = 0;
  OculusHMD::initialization(); // OculusHMD is a singleton for accessing the Oculus Device in a static way for better comfort
  handle_render_thread = CreateThread(NULL, 0,
	  runDirectX, &cont, 0, NULL);
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
	  // OculusHMD::instance()->render(cont.left_undistorted, cont.right_undistorted);
	  // *****************************************************************
    }
    // main control loop
    char key = waitKey(20);
    cont.handleKey(key);
  }
  // *****************************************************************
  delete OculusHMD::instance();
  dx11->CleanD3D();
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

// Multithreaded Version
DWORD WINAPI runDirectX(LPVOID lpArg)
{
	ARiftControl* arift_c = (ARiftControl*)lpArg;
	// arift_control->left_undistorted;
	// clear out the window class for use
	ZeroMemory(&dx11->window_class_, sizeof(WNDCLASSEX));

	// fill in the struct with the needed information
	dx11->window_class_.cbSize = sizeof(WNDCLASSEX);
	dx11->window_class_.style = CS_HREDRAW | CS_VREDRAW;
	dx11->window_class_.lpfnWndProc = WindowProc;
	dx11->window_class_.hInstance = GetModuleHandle(NULL);
	dx11->window_class_.hCursor = LoadCursor(NULL, IDC_ARROW);
	dx11->window_class_.hbrBackground = (HBRUSH)COLOR_WINDOW;
	dx11->window_class_.lpszClassName = "WindowClass1";

	// register the window class
	RegisterClassEx(&dx11->window_class_);

	// create the window and use the result as the handle
	dx11->window_ = CreateWindowEx(NULL,
		"WindowClass1",    // name of the window class
		"DirectX Render Scene",   // title of the window
		WS_OVERLAPPEDWINDOW,    // window style
		300,    // x-position of the window
		300,    // y-position of the window
		500,    // width of the window
		400,    // height of the window
		NULL,    // we have no parent window, NULL
		NULL,    // we aren't using menus, NULL
		GetModuleHandle(NULL),    // application handle
		NULL);    // used with multiple windows, NULL

	dx11->InitD3D();
	ShowWindow(dx11->window_, SW_SHOW);
	// display the window on the screen
	MSG msg;

	// wait for the next message in the queue, store the result in 'msg'
	while (GetMessage(&msg, NULL, 0, 0))
	{
		// translate keystroke messages into the right format
		TranslateMessage(&msg);

		// send the message to the WindowProc function
		DispatchMessage(&msg);
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


