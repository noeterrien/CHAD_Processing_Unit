# CHAD_Processing_Unit

## Requirements
To compile the CHAD Processing Unit (CPU), you will need the following dependencies :

* <u>OpenCV2</u> : See installation instructions for Linux at https://docs.opencv.org/4.x/d7/d9f/tutorial_linux_install.html. To install OpenCV required dependencies, you can use the provided script : use `source CV2_install_dependencies.sh`. **MAKE SURE OpenCV IS BUILD WITH GStreamer**. The provided script should install GStreamer dependencies but OpenCV might need additionnal configuration for the build. You can check if GStreamer is installed using a c++ script : Once executed, look for `Video I/O : GStreamer`. It should be set to `YES`
```
#include <opencv2/opencv.hpp>
#include <iostream>

std::cout << cv::getBuildInformation() << std::endl;
```
In case you need to rebuild OpenCV with GSTREAMER libraries enabled, use the following commands (from OpenCV repository) :
```
cmake -D WITH_GSTREAMER=ON \
      -D WITH_FFMPEG=ON \  # optional if you want only GStreamer
      -D CMAKE_BUILD_TYPE=Release \
      -D CMAKE_INSTALL_PREFIX=/usr/local \
      ..

make -j$(nproc)
sudo make install
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