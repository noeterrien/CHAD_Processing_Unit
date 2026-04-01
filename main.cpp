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

int main()
{
    std::string CAMSET = "udpsrc port=5600 ! application/x-rtp,media=video,encoding-name=H264 !  rtph264depay ! avdec_h264 ! videoconvert ! appsink";
    //std::string CAMSET = "tests/test_video.mp4";
    
    Camera cam1(CAMSET);
    //Camera cam1("tests/test_video.mp4");
    cam1.start();
    cv::Ptr<cv::SiftFeatureDetector> sift1 = cv::SiftFeatureDetector::create();
    cv::Ptr<cv::DescriptorMatcher> matcher1 = cv::DescriptorMatcher::create(cv::DescriptorMatcher::BRUTEFORCE);

    Display display("");

    FrameProcessor *fp_ref(0), *fp(0);
    cv::Mat frame_with_keypoints;

    float dx(0), dy(0), dz(0);

    Sender this_to_ROV("192.168.2.2", 1106);
    
    std::cout << "YAY" << std::endl;

    while (true) {

        if (cam1.is_new_frame_available())
        {
            float dx(0), dy(0), dz(0);
            // extract new frame and display it
            {
                // extract new frame  
                std::lock_guard<std::mutex> guard(cam1.lock_frame); // prevent cam1 frame from being changed while processing it
                fp = new FrameProcessor(cam1.get_current_frame(), sift1, matcher1);
                fp->computeKeypointsAndDescriptors();
        
                // set reference frame
                if (fp_ref == 0) { fp_ref = new FrameProcessor(*fp); fp_ref->frame_safeLock(); } 

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
