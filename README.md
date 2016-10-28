# ARift with LSD-SLAM integration
developing augmented reality version of the Oculus rift

## Usage
There are 3 user input modes: DEFAULT, MODEL, WORLD
Upon startup the input mode is set to DEFAULT.
Some keys perform the same action independent of the current user mode.
Some keys result in different behavior depending on the active input mode.

The following key mapping is indipendent of the input mode:
- 'm' changes the input mode 
      planned changes to a model are discarded upon mode switch
      (DEFAULT -> MODEL -> WORLD ->  DEFAULT) 
      (no action if last key was identical)
- '1'-'6' selects the step size for other actions
- '+','-' multiplies / divides the stepsize by 10
- 'f' write camera frame rates to console
- 'P','p' increase/decrease camera auto sensor speed
- 'o' print out left and right camera stream position and zoom as well as world translation offset

Below the input mode dependent keymapping is documented:
* DEFAULT
- 'w','a','s','d' change relative position of the camera feeds on both eyes
- 'q','e' change relative zoom of both camera feeds
- 'W','A','S','D' change absolute position of both camer feeds
- 'Q','E' change absolute zoom of both camera feeds
- 'r' recenter the virtual world on the current head pose
* MODEL
- 'w','a','s','d','q','e' change 3D position of currently highlighted virtual model
- 'W','A','S','D','Q','E' change 3D rotation of currently highlighted virtual model
- 'Backspace' reset current change to model
- 'Carriage return' apply current change to model permanently
- ',' , '.' select previous/next model and reset unsaved change before 
* WORLD
- 'w','a','s','d','q','e' change 3D position of all models

## Specifications of the development environments

### Hardware
Setup A / Setup B 
Processor:        Intel Core i7-2630QM 2.00 GHz / 3rd Generation Intel® Core™ i5-3337U (1.80GHz 1600MHz 3MB)
RAM:              8.0 GB DDR  
Graphics Card:    AMD Radeon HD 6900M (2 GB graphics memory) / NVIDIA GeForce 610M (1GB)
HMD:              Oculus Rift DK1
Cameras:          2x IDS uEye UI-1221LE-C-HQ
Mainboard:        3x USB (2.0 or better)
                  1x HDMI

  Without testing we cannot be sure, but we are very confident that any hardware configuration that is capable of running the Oculus World sample in real time with 30fps will be sufficient for this application in its current form.
### Software

Operating System: Windows 7 Professional (SP1) x64 / Windows 8 x64
IDE:              Visual Studio 2013 Community
                  avaliable here: https://www.visualstudio.com/en-us/products/visual-studio-community-vs.aspx 
                  (requires free registration for use afer 30 Days)
HMD driver:       Oculus Runtime
Camera Driver:    IDS uEye 
                  avaliable here: http://en.ids-imaging.com
Libraries:        Windows 8.x SDK (usually shipped with Visual Studio)
                  Oculus SDK 0.4.4 (will be updated/checked to work with current version soon)
                  IDS uEye API (shipped with the driver)
                  DirectXTK (avaliable here: https://directxtk.codeplex.com/releases/view/612864)
#### Camera Calibration Tools

Matlab
  OCamCalib: Omnidirectional Camera Calibration Toolbox for Matlab
  available here: https://sites.google.com/site/scarabotix/ocamcalib-toolbox

## Installation Setup
1. Download & Install Visual Studio 2013 Community
2. Download & Install IDS uEye camera driver
3. Download & Install Oculus Runtime and SDK
4. Download and unzip DirectXTK
  - to directory DirectXTK
5. Open provided solution file or create your own
6. add dependencies to project in the project properties
   - add the following include directories for the compiler (in debug **and** release) in "C/C++" -> "General" -> "Additional Include Directories"
    * LibOVR\Src
    * LibOVR\Include
    * opencv\build\include
    * DirectXTK\Inc
    * IDS\uEye\Develop\include
   - add the following library directories for the linker (in debug **and** relese) in "Linker" -> "General" -> "Additional Library Directories"
    * LibOVR\Lib\Win32\VS2013
   - add the following Libraries for the linker (in debug **and** release)  in "Linker" -> "Input" -> "Additional Dependencies"
    * uEye_api.lib; 
        (for the cameras)
    * ws2_32.lib;Dbghelp.lib;libovrd.lib;dxgi.lib;dxguid.lib;d3d10.lib;d3d11.lib;d3dcompiler.lib;winmm.lib;kernel32.lib;user32.lib;gdi32.lib;winspool.lib;comdlg32.lib;advapi32.lib;shell32.lib;ole32.lib;oleaut32.lib;uuid.lib;odbc32.lib;odbccp32.lib
        (for the oculus SDK)
    * kernel32.lib;user32.lib;gdi32.lib;winspool.lib;comdlg32.lib;advapi32.lib;shell32.lib;ole32.lib;oleaut32.lib;uuid.lib;odbc32.lib;odbccp32.lib
        (for WIN API)
  - Set "mixed" debugging in "Debugging" -> "Debugger Type" 
