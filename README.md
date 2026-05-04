# CHAD_Processing_Unit

The CHAD_Processing_Unit (CPU) processes images from a ROV camera, detecting keypoints to then compute translation between current frame and a reference frame. The translation is then transmitted back to the ROV using a custom version of ardupilot. See https://www.github.com/AlioTheCat/ardupilot.git

## Requirements
To compile the CHAD Processing Unit (CPU), you will need the following dependencies :
### OpenCV2
First install the required dependencies using the provided ```CV2_install_dependencies.sh``` script :
```bash
source CV2_install_dependencies.sh
```

Then install OpenCV2 minimal prerequisites, running ```sudo apt update && sudo apt install -y cmake g++ wget unzip```. You can then download and unpack sources (as found in OpenCV's documentation)
```bash
wget -O opencv.zip https://github.com/opencv/opencv/archive/4.x.zip
unzip opencv.zip
```
Finally configure, build and install OpenCV2 : 
```bash
cd opencv-4.x
mkdir build
cmake -D WITH_GSTREAMER=ON \
      -D WITH_FFMPEG=ON \  # optional if you want only GStreamer
      -D CMAKE_BUILD_TYPE=Release \
      -B build
cd build
make -j$(nproc)
sudo make install
```
**MAKE SURE OpenCV IS BUILT WITH GStreamer**. The provided script should install GStreamer dependencies but OpenCV might need additionnal configuration for the build. You can check if GStreamer is installed using a c++ script : Once executed, look for `Video I/O : GStreamer`. It should be set to `YES`
```c++
#include <opencv2/opencv.hpp>
#include <iostream>

std::cout << cv::getBuildInformation() << std::endl;
```

### cpp-mjpeg-streamer
nadjieb' `cpp-mjpeg-streamer` is used to display camera in a browser in real time.
To install, do :
```bash
git clone https://github.com/nadjieb/cpp-mjpeg-streamer.git;
cd cpp-mjpeg-streamer;
mkdir build && cd build;
cmake ../;
make;
sudo make install;
```

### cpp-httplib
This library is used to communicate with the CHAD Processing Unit via http requests (usually using cockpit).
To install, do :
```bash
git clone https://github.com/yhirose/cpp-httplib.git
cd cpp-httplib
mkdir build && cd build
cmake ../
make 
sudo make install
```

### yaml-cpp
This library is used to parse the configuration file `config.yaml`
```bash
git clone https://github.com/jbeder/yaml-cpp.git
cd yaml-cpp
mkdir build && cd build
cmake -DYAML_BUILD_SHARED_LIBS=on ..
make 
sudo make install
```

## Compiling
It is **strongly** advised to use a linux environment to run the CPU and the custom version of ardupilot associated. One may use wsl but be aware that connection issues will arise concerning camera image fetching and/or measurement sending to the ROV due to the virtual environment having to mirror the connection.

*cmake* is used to compile the CPU : First, go into the CPU folder. When compiling for the first time, you will have to run the following command to configure the build folder :
```bash
cmake -B build
```

Then, when you want to build it, you can run
```bash
cmake --build build
```

To run the program, then simply use (make sure your camera source is running)
```bash
./build/CPU
```

## How it works

The full pipeline is available in the ```main.cpp``` file. It is built on three main libraries that you can find in the libraries folder : 

### Camera
Camera objects are used to fetch images from a video or a webcam. They are declared with the following arguments 
      * a source (string) : a gstreamer pipeline or a video address; Calls the constructor for OpenCV VideoCapture object in the background.
      * a pointer to a frame (cv::Mat) : This is where the frames will be stored
      * a pointer to a mutex (std::mutex) : This is used to ensure that current frame cannot be accessed while it is being written to

To start running the Camera, use the method ```start()```

### FrameProcessor
This is where the magic happens. Instances of FrameProcessor take a pointer to a frame to be declared, as well as pointers to cv::SiftFeatureDetector and cv::DescriptorMatcher to be declared. This way, the frame can be modified by another thread (such as the Camera thread) before calling ```computeKeypointsAndDescriptors()``` method and static function ```computeTranslation``` to compute the translation between the current frame and the reference frame (given as another FrameProcessor object). When calling ```computeKeypointsAndDescriptors()```, the frame of the FrameProcessor should be locked using for instance a mutex and a lock_guard to ensure it is not modified during the process.

The camera coordinates system is : 
* x : horizontal axis, left to right
* y : vertical axis, top to bottom
* z : x $\wedge$ y

The translation is computed using keypoints location in both frames for x and y (and thus the result is in pixels) while the intensity of the red channel is used for z information (thus not the same unit).

Other convenient methods are provided in ```FrameProcessor.hpp```

### Interface 
The Interface library provides objects to interact with the CPU and transmit information to ardupilot.
* Display : used to display frames (cv::Mat) to a web page using http protocol.
* Sender : used to send measurements to ardupilot using udp protocol.
* HTTPServer : used to communicate with the CPU using http requests.

### Other libraries
The ```Parameters``` library is used to parse config from the ```config.yaml```, and store the parameters so that they can be modified while the program is runnning using, for insance, http requests.

The ```utilitaries``` library is self explanatory.

## TODO and possible avenues to explore

* Segmentation faults can happen when reseting the reference frame or changing parameters while the CPU is running. Should be fixed using mutex and lock_guards.

* Red channel only works when the light comes mainly from the ROV lights. Other ways of computing depth should be explored.

* Ideally, one would want to be able to compute rotation to apply and not only translation.