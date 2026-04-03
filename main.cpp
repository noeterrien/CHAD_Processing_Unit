#include <Display.hpp>
#include <Camera.hpp>
#include <FrameProcessor.hpp>
#include "utilitaries.cpp"
#include <Interface.hpp>

#include <mutex>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <time.h>
#include <string>
#include <httplib.h>

int main()
{
    ////////////////////////////////////////////////////   INITIALIZATION   ////////////////////////////////////////////////////
    
    // Camera Initialization
    cv::Mat *frame_cam1 = new cv::Mat(); // a pointer indicating where frames from cam1 should be stored to be accessed by other instances
    std::mutex *lock_frame_cam1 = new std::mutex(); // a mutex used to lock the previous pointer so that two thread don't try to access it at the same time
    //std::string CAMSET = "udpsrc port=5600 ! application/x-rtp,media=video,encoding-name=H264 !  rtph264depay ! avdec_h264 ! videoconvert ! appsink";
    //Camera cam1(CAMSET, frame_cam1, lock_frame_cam1);
    Camera cam1("tests/test_video.mp4", frame_cam1, lock_frame_cam1);
    cam1.start();

    // Frame Processing Initialization
    cv::Ptr<cv::SiftFeatureDetector> sift1 = cv::SiftFeatureDetector::create();
    cv::Ptr<cv::DescriptorMatcher> matcher1 = cv::DescriptorMatcher::create(cv::DescriptorMatcher::BRUTEFORCE);
    FrameProcessor *fp_ref(0), *fp(0);
    cv::Mat frame_with_keypoints;

    // Display Initialization
    Display display("");

    // this to ROV interface initialization
    float dx(0), dy(0), dz(0);
    Sender this_to_ROV("192.168.2.2", 1106);

    // http requests handling interface initialization
    HTTPServer incoming_requests_handler;
    incoming_requests_handler.Post("/reset_reference_frame", [&](const httplib::Request& req, httplib::Response& res){ // define reset_reference_frame post request
        if (fp_ref != 0) {
            delete fp_ref; 
            fp_ref = 0;
            std::cout << "Deleted reference frame" << std::endl;
        }
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content("Reference frame reset", "text/plain");
    });
    incoming_requests_handler.start(8000); // listen from all IPs on port 8080

    ////////////////////////////////////////////////////   PROCESSING   //////////////////////////////////////////////////////////

    while (true) {

        if (cam1.is_new_frame_available())
        {
            float dx(0), dy(0), dz(0);
            // extract new frame and display it
            {
                // extract new frame  
                std::lock_guard<std::mutex> guard(*lock_frame_cam1); // prevents cam1 frame from being changed while processing it
                cam1.set_new_frame_available_status(false);
                fp = new FrameProcessor(frame_cam1, sift1, matcher1);
                fp->computeKeypointsAndDescriptors();
                std::cout << "number of keypoints : " << fp->get_numberOfKeypoints() << std::endl;
        
                // set reference frame
                if (fp_ref == 0) 
                { 
                    fp_ref = new FrameProcessor(*fp); 
                    fp_ref->frame_safeLock(); 
                    std::cout << "Reference frame set : number of keypoints = " << fp_ref->get_numberOfKeypoints() << std::endl;
                } 

                // DEBUG : safelock frame
                fp->frame_safeLock();

                // display
                fp->drawKeypoints(frame_with_keypoints);
            }

            // compute translation from reference frame
            if (fp_ref != 0) {
                FrameProcessor::computeTranslation(*fp_ref, *fp, dx, dy, dz);
            }

            // send the result to ROV
            this_to_ROV.sendVector(-dx, -dy, -dz);
            std::cout << "sent " << -dx << ", " << -dy << ", " << -dz << std::endl;
        
            drawArrowFromOrigin(frame_with_keypoints, dx, dy);
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
