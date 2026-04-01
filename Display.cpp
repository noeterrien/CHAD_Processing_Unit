#include <Display.hpp>

#include <iostream>
#include <string>

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