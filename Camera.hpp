#ifndef DEF_CAMERA
#define DEF_CAMERA

#include <thread>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

class Camera 
{
public :
    Camera(std::string src, cv::Mat *frame, std::mutex *lock_frame); // src can either be the path to a mp4 file or ip_address:port
    Camera(std::string ip_address, std::string port, cv::Mat *frame, std::mutex *lock_frame); // for convenience

    void start(); // starts frame gathering as a background task
    void set_new_frame_available_status(bool still_considered_new);
    bool is_new_frame_available();

    
private :
    cv::VideoCapture stream;
    cv::Mat *current_frame;
    bool stream_opened;

    std::thread thread;
    std::mutex *lock_frame; // used to lock memory while processing image
    bool new_frame_available;
    
    void run(); // loop to read the last frame received and update new_frame_available status
};


#endif