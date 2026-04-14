#include <Interface.hpp>

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <httplib.h>
#include <iostream>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>

Display::Display(std::string _path) {
    streamer.start(8080);
    path = _path;
    std::cout << "Stream started. Visit http://localhost:8080/" << path << " to watch" << std::endl;
}

void Display::post(cv::Mat const& frame) {
    if(!streamer.isRunning()){
        std::cerr << "Stream is not running";
        exit(EXIT_FAILURE); //continue -> TODO : demander à Luka ce que ça fait
    }

    if(frame.empty()) {
        std::cerr << "Frame is empty";
        exit(EXIT_FAILURE); //continue
    }

    std::vector<uchar> buff_bgr;
    cv::imencode(".jpg", frame, buff_bgr, params);
    streamer.publish("/"+path, std::string(buff_bgr.begin(), buff_bgr.end()));
}

Display::~Display() {
    streamer.stop();
}

void Display::post_vmultiple(std::vector<cv::Mat> const& frames, int cv_type) {

    // find maximum width and total height
    int max_width = 0;
    int total_height = 0;
    for (auto it=frames.begin() ; it != frames.end() ; it++) {
        max_width = it->size().width > max_width ? it->size().width : max_width;
        total_height += it->size().height;
    }

    // create and fill resulting matrix
    cv::Mat to_post(total_height, max_width, cv_type, cv::Scalar(255,255,255));
    int current_height = 0;
    for (auto it=frames.begin() ; it != frames.end() ; it++) {
        it->convertTo(to_post(cv::Range(current_height, current_height + it->size().height),
                              cv::Range(0, it->size().width)), 
                      cv_type);
        current_height += it->size().height;
    }

    // post
    post(to_post);
}

Sender::Sender(std::string ip_address, uint16_t port) {
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(ip_address.c_str());
    address.sin_port = htons(port);

    size = sizeof(address);

    _socket = socket(AF_INET, SOCK_DGRAM, 0);
}

Sender::~Sender() { close(_socket); }

void Sender::sendVector(float x, float y, float z)
{   
    float vector[3] = {x, y, z};
    sendto(_socket, &vector, sizeof(vector), MSG_DONTWAIT, (const sockaddr*) &address, size);
}

void HTTPServer::start(uint16_t port, std::string address) {
    std::thread thread([=]{ 
        std::cout << "starting http server on " << address << ":" << port << std::endl;

        int ret = listen(address, port);
    });
    thread.detach();
    std::cout << "started HTTP server, waiting for incoming requests" << std::endl;
}