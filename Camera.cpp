#include <Camera.hpp>

#include <opencv2/opencv.hpp>
#include <iostream>
#include <thread>
#include <mutex>

Camera::Camera(std::string src) : new_frame_available(false)
{

    std::cout << "Attempting to fetch signal from source " << src << std::endl;

	stream.open(src, cv::CAP_GSTREAMER);
	if (stream.isOpened()) 
    {
		std::cout << "Camera started from source" << src << std::endl;
        stream_opened = true;
        current_frame = new cv::Mat;
	} else 
    {
		std::cerr << "Could not open camera from source" << std::endl;
        stream_opened = false;
        exit(EXIT_FAILURE);
	}

}

Camera::Camera(std::string ip_address, std::string port) : Camera(ip_address + ":" + port) {};

void Camera::start() {
    if (stream_opened) 
    {
        thread = std::thread(&Camera::run, this);
	    thread.detach();
    } else 
    {
        std::cerr << "Stream is not open. Cannot start fetching frames" << std::endl;
        exit(EXIT_FAILURE);
    }
}

Camera::~Camera() { delete current_frame; }

void Camera::run()
{
    cv::Mat frame_temp;
	while (stream_opened) {
		bool new_frame_temp_available = stream.read(frame_temp);
        {
            std::lock_guard<std::mutex> guard(lock_frame); // prevents copy if frame is being processed
            frame_temp.copyTo(*current_frame);
            new_frame_available = new_frame_temp_available;
        }
	}
}

cv::Mat *Camera::get_current_frame() 
{
    new_frame_available = false;
    return current_frame;
}

bool Camera::is_new_frame_available() { return new_frame_available; }