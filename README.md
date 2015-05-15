# ARift
developing augmented reality version of the Oculus rift


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
                  OpenCV 2.4.10 (obsolete, will be removed soon, needed to compile current version)
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
5. Download & Install OpenCV
  - unzip to directory "..\opencv"
  - add the directory "opencv\build\x86\vc12\bin" to the system variable "Path"
    (to do so go to "Start" -> "System" -> "Advanced System Settings")
6. Open provided solution file or create your own
7. add dependencies to project in the project properties
   - add the following include directories for the compiler (in debug **and** release) in "C/C++" -> "General" -> "Additional Include Directories"
    * LibOVR\Src
    * LibOVR\Include
    * opencv\build\include
    * DirectXTK\Inc
    * IDS\uEye\Develop\include
   - add the following library directories for the linker (in debug **and** relese) in "Linker" -> "General" -> "Additional Library Directories"
    * LibOVR\Lib\Win32\VS2013
    * opencv\build\x86\vc12\lib
   - add the following Libraries for the linker (in debug **and** release)  in "Linker" -> "Input" -> "Additional Dependencies"
    * uEye_api.lib; 
        (for the camera)
    * ws2_32.lib;Dbghelp.lib;libovrd.lib;dxgi.lib;dxguid.lib;d3d10.lib;d3d11.lib;d3dcompiler.lib;winmm.lib;kernel32.lib;user32.lib;gdi32.lib;winspool.lib;comdlg32.lib;advapi32.lib;shell32.lib;ole32.lib;oleaut32.lib;uuid.lib;odbc32.lib;odbccp32.lib
        (for the oculus SDK)
    * kernel32.lib;user32.lib;gdi32.lib;winspool.lib;comdlg32.lib;advapi32.lib;shell32.lib;ole32.lib;oleaut32.lib;uuid.lib;odbc32.lib;odbccp32.lib
        (for WIN API)
   - add the following Libraries for the linker **debug**  in "Linker" -> "Input" -> "Additional Dependencies"
    * opencv_core2410d.lib;opencv_imgproc2410d.lib;opencv_highgui2410d.lib;opencv_ml2410d.lib;opencv_video2410d.lib;opencv_features2d2410d.lib;opencv_calib3d2410d.lib;opencv_objdetect2410d.lib;opencv_contrib2410d.lib;opencv_legacy2410d.lib;opencv_flann2410d.lib
        (for opencv)
   - add the following Libraries for the linker **release**  in "Linker" -> "Input" -> "Additional Dependencies"
    * opencv_core2410.lib;opencv_imgproc2410.lib;opencv_highgui2410.lib;opencv_ml2410.lib;opencv_video2410.lib;opencv_features2d2410.lib;opencv_calib3d2410.lib;opencv_objdetect2410.lib;opencv_contrib2410.lib;opencv_legacy2410.lib;opencv_flann2410.lib
  - Set "mixed" debugging in "Debugging" -> "Debugger Type" 
