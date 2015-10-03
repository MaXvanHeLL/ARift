
// ARift includes
#include "include/ARiftControl.h"
#include "include/OculusHMD.h"
#include "include/GraphicsAPI.h"
#include "include\Helpers.h"
#include <iostream>
#include <string>
#include <windows.h>

using namespace std;
// ********************************************************************************
DWORD WINAPI directXHandling(LPVOID lpArg);
void render(ARiftControl* arift_c);
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

#define SHOW_FPS false
const bool FULL_SCREEN = false;
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
    cont.init(dx11);
    std::cout << "init done" << std::endl;
  }  

	// start the Render Thread
  handle_render_thread = CreateThread(NULL, 0,
	  directXHandling, &cont, 0, NULL);
  
  std::cout << "Starting Camera loop" << std::endl;
  cont.start();
  while(cont.keepRunning())
  {
		if (AR_HMD_ENABLED)
		{
		}
	}

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

