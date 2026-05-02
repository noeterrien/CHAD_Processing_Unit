#include <Camera.hpp>

#include <opencv2/opencv.hpp>
#include <iostream>
#include <thread>
#include <mutex>

Camera::Camera(std::string src, cv::Mat *frame, std::mutex *lock_frame) : new_frame_available(false), current_frame(frame), lock_frame(lock_frame)
{

    std::cout << "Attempting to fetch signal from source " << src << std::endl;

	stream.open(src, cv::CAP_GSTREAMER);
	if (stream.isOpened()) 
    {
		std::cout << "Camera started from source" << src << std::endl;
        stream_opened = true;
	} else 
    {
		std::cerr << "Could not open camera from source" << src << std::endl;
        stream_opened = false;
        exit(EXIT_FAILURE);
	}

}

void Camera::start()
{
    cv::Mat frame_temp;
	while (stream_opened) {
		bool new_frame_temp_available = stream.read(frame_temp);
        {
            std::lock_guard<std::mutex> guard(*lock_frame); // prevents copy if frame is being processed
            frame_temp.copyTo(*current_frame);
            new_frame_available = new_frame_temp_available;
        }
	}
}

void Camera::set_new_frame_available_status(bool still_considered_new) { new_frame_available = still_considered_new; }

bool Camera::is_new_frame_available() { return new_frame_available; }