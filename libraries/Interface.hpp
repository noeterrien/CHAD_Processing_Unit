#ifndef DEF_INTERFACE
#define DEF_INTERFACE

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <httplib.h>
#include <opencv2/opencv.hpp>
#include <nadjieb/mjpeg_streamer.hpp>
#include <thread>
#include <string>
#include <vector>

using MJPEGStreamer = nadjieb::MJPEGStreamer;

/*Creates a server that can be used to access real time frame processing display*/
class Display 
{
public :

    Display(std::string path);
    void post(cv::Mat const& frame); // post a frame to the web display page
    void post_vmultiple(std::vector<cv::Mat> const& frames, int cv_type); // post multiple frames vertically. All frames are converted to cv_type 
    ~Display();

private :
    MJPEGStreamer streamer;
    std::string path; // html path frames should be posted to
    std::vector<int> params{cv::IMWRITE_JPEG_QUALITY, 90};
};

/*An instance to send messages to an address using use IPv4 (AF_INET) and UDP protocol (SOCK_DGRAM)*/
class Sender {

public:
    Sender(std::string ip_address, uint16_t port);
    ~Sender();

    void sendVector(float x, float y, float z);

private:
    sockaddr_in address;
    socklen_t size;
    int _socket;
};

/*An instance to handle HTTP requests*/
class HTTPServer : public httplib::Server{

public:
    void start(uint16_t port, std::string address="0.0.0.0"); // starts listening on another thread
};

#endif