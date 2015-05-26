# ARift
developing augmented reality version of the Oculus rift
## User Interface and Keymapping 

- The movement rate can be set using the keys '1' (= 0.1 pixel/step), '2' (= 0.2 px/st , standart), '3' (= 0.5 px/st), '4' (= 1 px/st), '5' (= 5 px/st).
- 'o': Write offest in camera input, virtual eye distance and virtual world offset to console 
- The zoom can be adjusted via 'z' and 'Z'.
- 'v' Recenters the  virtual world to the current view.
- 'm' switches through modes: default mode -> model mode -> camera mode -> world mode.
   repeated presses are ignored,  requires any other keypress inbetween to work again
   * default mode: nothing is highlighted
   * model mode: current model is highlighted
   * camera mode: nothing is highlighted
   * world mode: all models are highligted 
- The images can be moved with relation to the screen using 'W','A','S','D'.
- 'w','a','s','d' :
  * In default mode: The images can be moved with relation to each other
  * In model mode: The current model x,y-position is adjusted
  * In world mode: all models x,y-position is adjusted
- 'y','x':
  * model mode: adjust z-position of the current model
  * world mode: adjust z-position of the all models
- In model mode 'q','e' adjust y-rotation of the model (in model coordinates)
- In model mode 't' turns on/off auto z-translation of the model, requires any other keypress inbetween to work again
- In model mode 'r' turns on/off auto y-rotation of the model, requires any other keypress inbetween to work again
- 'c' and 'C' change offset of virtual cameras along the y-axis (i.e. adjust virutal eye distance)

- The current frames per second can be written to console by pressing 'f'
- The speed of auo sensore shutter can adjusted via 'p' (slower), 'P' (faster). Standard value is the maximum of 100. 

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
