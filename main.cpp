
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include "include/ARiftControl.h"
#include "include/OculusHMD.h"
#include "include/GraphicsAPI.h"
#include <iostream>
#include <windows.h>
#define   OVR_D3D_VERSION 11
#include "OVR_CAPI_D3D.h"

using namespace cv;
using namespace std;
// ********************************************************************************
DWORD WINAPI directXHandling(LPVOID lpArg);
void render(ARiftControl* arift_c);
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

GraphicsAPI* dx11 = NULL;
// ********************************************************************************

int main(int, char**)
{
  ARiftControl cont;
  cont.init();

  // install the Oculus Rift and GraphicsAPI Renderer and init Render Thread
  // *****************************************************************
  dx11 = new GraphicsAPI();
  HANDLE handle_render_thread = 0;
  OculusHMD::initialization(); // OculusHMD is a singleton for accessing the Oculus Device in a static way for better comfort
  OculusHMD::instance()->setRenderer(dx11);
  OculusHMD::instance()->configureStereoRendering();

  handle_render_thread = CreateThread(NULL, 0,
	  directXHandling, &cont, 0, NULL);
  // *****************************************************************

  namedWindow("undist",1);
  namedWindow("both", CV_WINDOW_FULLSCREEN);
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
    }
    // main control loop
    char key = waitKey(20);
    cont.handleKey(key);
  }
  // *****************************************************************
  delete OculusHMD::instance();
  dx11->CleanD3D();
  delete dx11;
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
DWORD WINAPI directXHandling(LPVOID lpArg)
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
		20,    // x-position of the window
		20,    // y-position of the window
		RIFT_RESOLUTION_WIDTH,    // width of the window
		RIFT_RESOLUTION_HEIGHT,    // height of the window
		NULL,    // we have no parent window, NULL
		NULL,    // we aren't using menus, NULL
		GetModuleHandle(NULL),    // application handle
		NULL);    // used with multiple windows, NULL

	dx11->InitD3D();
	ShowWindow(dx11->window_, SW_SHOW);
	// display the window on the screen
	MSG msg;

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
		render(arift_c);
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


void render(ARiftControl* arift_c)
{
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = RIFT_RESOLUTION_WIDTH / 2;
	desc.Height = RIFT_RESOLUTION_HEIGHT / 2;
	desc.MipLevels = desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;

	int pixelSize = sizeof(int);//pixel size. Each pixels are represented by a int 32bits.
	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = arift_c->left_undistorted.data; //pixel buffer
	data.SysMemPitch = pixelSize * RIFT_RESOLUTION_WIDTH / 2;// line size in byte
	data.SysMemSlicePitch = pixelSize * RIFT_RESOLUTION_WIDTH/ 2 * RIFT_RESOLUTION_HEIGHT/ 2;// total buffer size in byte

	// dx11->pTexture = dx11->dev->CreateTexture2D(&desc, );
}


