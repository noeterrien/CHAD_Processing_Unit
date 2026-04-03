# CHAD_Processing_Unit

## Requirements
To compile the CHAD Processing Unit (CPU), you will need the following dependencies :

* <u>OpenCV2</u> : First install the required dependencies using the provided ```CV2_install_dependencies.sh``` script. Then install OpenCV2 minilal prerequisites, running ```sudo apt update && sudo apt install -y cmake g++ wget unzip```. You can then download and unpack sources : 
```
wget -O opencv.zip https://github.com/opencv/opencv/archive/4.x.zip
unzip opencv.zip
```
Finally configure, build and install OpenCV2 : 
```
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
```
#include <opencv2/opencv.hpp>
#include <iostream>

std::cout << cv::getBuildInformation() << std::endl;
```

* <u>cpp-mjpeg-streamer</u> : nadjieb' cpp-mjpeg-streamer is used to display camera in a browser in real time. To install, do :
```
git clone https://github.com/nadjieb/cpp-mjpeg-streamer.git;
cd cpp-mjpeg-streamer;
mkdir build && cd build;
cmake ../;
make;
sudo make install;
```

* <u> cpp-httplib </u> : this library is used to communicate with the CHAD Processing Unit via http request (usually using cockpit). To install, do :
```
git clone https://github.com/yhirose/cpp-httplib.git
cd cpp-httplib
mkdir build && cd build
cmake ../
make 
sudo make install
```