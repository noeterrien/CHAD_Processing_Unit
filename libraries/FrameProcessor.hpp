#pragma once

#include <opencv2/opencv.hpp>
#include <vector>

class FrameProcessor {

public:
	FrameProcessor(cv::Mat *frame, cv::Ptr<cv::SiftFeatureDetector> sift, cv::Ptr<cv::DescriptorMatcher> matcher);
	
	~FrameProcessor();
	// necessary to avoid having to copy camera frames to process them
	FrameProcessor(const FrameProcessor &other);
	FrameProcessor& operator=(const FrameProcessor &other);

	void setMask(cv::Mat &_mask);
	void computeKeypointsAndDescriptors();

	static void computeTranslation(FrameProcessor fp1, FrameProcessor fp2, float &dx, float &dy, float &dz); /*fp1 and fp2 frames can be changed while 
																											   translation is being computed, as long as keypoints
																											   and descriptors have already been computed*/
	
	// methods for debug
	void frame_safeLock(); // copies the associated frame so that it cannot be modified by another thread (for DEBUG purposes)
	size_t get_numberOfKeypoints();
	void get_frame(cv::Mat &output_frame); // if the frame is safe locked, returns the frame

	// methods for display
	void drawKeypoints(cv::Mat &outFrame);


private :
	cv::Mat *frame;


	cv::Mat *mask; // a mask to use only part of the frame
	std::vector<cv::KeyPoint> keypoints; // a KeyPoint as Point2f attribute pt from which we can extract x and y attributes
	cv::Mat descriptors; // descriptors of keypoints. Index in keypoints vector corresponds to index of descriptors
	std::vector<uint8_t> keypoints_red_component; // red_channel == 0 or 255 indicates saturation. In that case, keypoint should not be used

	cv::Ptr<cv::SiftFeatureDetector> sift;
	cv::Ptr<cv::DescriptorMatcher> matcher;

	static void matchKeypoints(FrameProcessor fp1, FrameProcessor fp2, std::vector<cv::DMatch> &matches); // fp1 and fp2 frames can be changed during matching

	bool frameCopied;
};