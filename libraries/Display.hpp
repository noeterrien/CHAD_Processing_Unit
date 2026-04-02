#ifndef DEF_DISPLAY
#define DEF_DISPLAY

#include <opencv2/opencv.hpp>
#include <nadjieb/mjpeg_streamer.hpp>
#include <string>
#include <vector>

using MJPEGStreamer = nadjieb::MJPEGStreamer;

class Display 
{
public :

    Display(std::string path);
    void post(cv::Mat const& frame); // post a frame to the web display page
    ~Display();

private :
    MJPEGStreamer streamer;
    std::string path; // html path frames should be posted to
    std::vector<int> params{cv::IMWRITE_JPEG_QUALITY, 90};
};

#endif