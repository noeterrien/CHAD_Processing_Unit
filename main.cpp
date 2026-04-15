////////////////////////////////////////////////////////   IMPORTS   ///////////////////////////////////////////////////////////

#include <Camera.hpp> // a library to read frames from a given source (either camera or video)
#include <FrameProcessor.hpp> // main module, used to compute keypoints and translation between a given frame and the reference frame
#include "utilitaries.cpp"
#include <Interface.hpp> // module used to handle web communications (includes frame processing display, sending data via udp and http requests handler)
#include <Parameters.hpp> // loads and parses parameters from config file

#include <mutex>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <time.h>
#include <string>
#include <httplib.h>

// Loading parameters as a global variable
Parameters params("config.yaml");

int main()
{
    ////////////////////////////////////////////////////   INITIALIZATION   ////////////////////////////////////////////////////

    // Camera Initialization
    cv::Mat *frame_cam1 = new cv::Mat(); // a pointer indicating where frames from cam1 should be stored to be accessed by other instances
    std::mutex *lock_frame_cam1 = new std::mutex(); // a mutex used to lock the previous pointer so that two thread don't try to access it at the same time
    Camera cam1(params.get<std::string>("camera_source"), frame_cam1, lock_frame_cam1);
    cam1.start();

    // Frame Processing Initialization
    cv::Ptr<cv::SiftFeatureDetector> sift1 = cv::SiftFeatureDetector::create();
    cv::Ptr<cv::DescriptorMatcher> matcher1 = cv::DescriptorMatcher::create(cv::DescriptorMatcher::BRUTEFORCE);
    FrameProcessor *fp_ref(0), *fp(0);
    std::mutex lock_reference_frame_processor;

    cv::Mat frame_with_keypoints;

    // Display Initialization
    Display display("", params.get<int>("udp_display_port"));

    // this to ROV interface initialization
    float dx(0), dy(0), dz(0);
    int kp_num(0), m_kp_num(0); // kp_num = number of keypoints in last processed frame
                                  // m_kp_num = number of matched keypoints between last processed frame and reference frame
    Sender this_to_ROV(params.get<std::string>("rov_ip"), params.get<int>("CHAD_sensor_port"));

    // http requests handling interface initialization
    HTTPServer http_requests_handler;
    http_requests_handler.set_default_headers({
        {"Access-Control-Allow-Origin", "*"}, // if you know from which address you want to access it, safer to replace "*" with "your_adress"
        {"Access-Control-Allow-Methods", "GET, POST, OPTIONS"},
        {"Access-Control-Allow-Headers", "Content-Type"}
    });
    http_requests_handler.Post("/reset_reference_frame", [&](const httplib::Request& req, httplib::Response& res){ // define reset_reference_frame post request
        if (fp_ref != 0) {
            std::lock_guard<std::mutex> guard(lock_reference_frame_processor);
            delete fp_ref;
            fp_ref = 0;
            std::cout << "Deleted reference frame" << std::endl;
        }
        res.set_content("Reference frame reset", "text/plain");
    });
    http_requests_handler.Get("/status/:param_id", [&](const httplib::Request& req, httplib::Response& res){
        auto param_id = req.path_params.at("param_id");
        if (param_id == "dx") res.set_content(to_string(dx), "text/plain");
        if (param_id == "dy") res.set_content(to_string(dy), "text/plain");
        if (param_id == "dz") res.set_content(to_string(dz), "text/plain");
        if (param_id == "kp_num") res.set_content(to_string(kp_num), "text/plain");
        if (param_id == "m_kp_num") res.set_content(to_string(m_kp_num), "text/plain");
    });
    http_requests_handler.start(params.get<int>("http_request_handler_port")); // listen from all IPs on port 8000

    ////////////////////////////////////////////////////   PROCESSING   //////////////////////////////////////////////////////////

    while (true) {

        if (cam1.is_new_frame_available())
        {
            // extract new frame and display it
            {
                // extract new frame  
                std::lock_guard<std::mutex> guard(*lock_frame_cam1); // prevents cam1 frame from being changed while processing it
                cam1.set_new_frame_available_status(false);
                fp = new FrameProcessor(frame_cam1, sift1, matcher1);
                fp->computeKeypointsAndDescriptors();
                kp_num = fp->get_numberOfKeypoints();
                // std::cout << "number of keypoints : " << kp_num << std::endl;
        
                // set reference frame
                {
                    std::lock_guard<std::mutex> guard(lock_reference_frame_processor);
                    if (fp_ref == 0) { fp_ref = new FrameProcessor(*fp); fp_ref->frame_safeLock(); std::cout << "Reference frame set : number of keypoints = " << fp_ref->get_numberOfKeypoints() << std::endl;}
                }
                 

                // DEBUG : safelock frame
                fp->frame_safeLock();

                // display
                fp->drawKeypoints(frame_with_keypoints);
            }

            // compute translation from reference frame
            if (fp_ref != 0) {
                m_kp_num = FrameProcessor::computeTranslation(*fp_ref, *fp, dx, dy, dz);
            }

            // send the result to ROV
            this_to_ROV.sendVector(dx, dy, dz);
            // std::cout << "sent " << dx << ", " << dy << ", " << dz << std::endl;
        
            drawArrowFromOrigin(frame_with_keypoints, dx, dy);
            cv::putText(frame_with_keypoints, "sent : " + std::to_string(dx) + ", " + std::to_string(dy) + ", " + std::to_string(dz), 
                        cv::Point(100, 500), 
                        cv::FONT_HERSHEY_SIMPLEX,
                        0.5,
                        cv::Scalar(0, 255, 255));
            display.post(frame_with_keypoints);

            // delete pointer and set its address to 0
            delete fp;
            fp = 0;
        }
        
    }

    delete fp_ref;
    fp_ref = 0;
    return 0;
}
