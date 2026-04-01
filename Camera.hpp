#ifndef DEF_CAMERA
#define DEF_CAMERA

#include <thread>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

class Camera 
{
public :
    Camera(std::string src); // src can either be the path to a mp4 file or ip_address:port
    Camera(std::string ip_address, std::string port); // for convenience
    ~Camera();

    void start(); // starts frame gathering as a background task
    cv::Mat *get_current_frame(); // returns a pointer to current frame and changes status of new_frame_available to false
    bool is_new_frame_available();

    std::mutex lock_frame; // used to lock memory while processing image

private :
    cv::VideoCapture stream;
    cv::Mat *current_frame;
    bool stream_opened;

    std::thread thread;
    bool new_frame_available;
    
    void run(); // loop to read the last frame received and update new_frame_available status
};


#endif