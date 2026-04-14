#include <FrameProcessor.hpp>

#include <opencv2/opencv.hpp>
#include <iostream>
#include "utilitaries.cpp"
#include <cmath>


FrameProcessor::FrameProcessor(cv::Mat *_frame, 
							   cv::Ptr<cv::SiftFeatureDetector> _sift, 
							   cv::Ptr<cv::DescriptorMatcher> _matcher) : frame(_frame), sift(_sift), matcher(_matcher), frameCopied(false)
{
	mask = new cv::Mat(cv::Mat::ones(frame->size(), CV_8UC1)); // default mask comprises the entire frame
}

FrameProcessor::~FrameProcessor() { delete mask; if(frameCopied){ delete frame; } }

// copy constructor
FrameProcessor::FrameProcessor(const FrameProcessor &other) : mask(new cv::Mat(*(other.mask))),
															  keypoints(other.keypoints),
															  descriptors(other.descriptors.clone()),
															  keypoints_red_component(other.keypoints_red_component), 
															  sift(other.sift), 
															  matcher(other.matcher) {
																if (other.frameCopied) {
																	frame = new cv::Mat(*(other.frame));
																	frameCopied = true;
																} else {
																	frame = other.frame;
																	frameCopied = false;
																}
															  }

// assignment (=) operator
FrameProcessor& FrameProcessor::operator=(const FrameProcessor &other){
    if (this != &other) {
        mask = new cv::Mat(*(other.mask));
        keypoints = other.keypoints;
        descriptors = other.descriptors.clone();
        keypoints_red_component = other.keypoints_red_component;
		sift = other.sift;
		matcher = other.matcher;
		if (other.frameCopied) {
			frame = new cv::Mat(*(other.frame));
			frameCopied = true;
		} else {
			frame = other.frame;
			frameCopied = false;
		}
    }
    return *this;
}

void FrameProcessor::setMask(cv::Mat &_mask) { mask = &_mask; }

void FrameProcessor::computeKeypointsAndDescriptors() 
{ 
	// compute keypoints and descriptors
	sift -> detectAndCompute(*frame, *mask, keypoints, descriptors);
	
	// red channel extraction
	for (auto kp = begin(keypoints); kp != end(keypoints); ++kp) {
		if (kp->pt.x >= 0 && kp->pt.x < frame->cols && kp->pt.y >= 0 && kp->pt.y < frame->rows) //  
		{
    		keypoints_red_component.push_back(frame->at<cv::Vec3b>(kp->pt)[2]);
		} else 
		{
			keypoints_red_component.push_back(0); // should avoid using kp with red channel == 0 for further treatment
		}
	}
}

void FrameProcessor::drawKeypoints(cv::Mat &outFrame) { cv::drawKeypoints(*frame, keypoints, outFrame, cv::Scalar(0, 127, 0)); }


/* Computes matches between keypoints of two given FrameProcessor objects and returns a vector giving these matches via
the matches reference. This vector is a vector of cv::DMatch objects. cv::DMatch objects each represent one match and have 
3 public attributes of interest :
	- distance : the distance between the descriptors of the two matched keypoints
	- queryIdx : the index of the keypoint in fp1 keypoints vector
	- trainIdx : the index of the keypoint in fp2 keypoints vector
Uses the matcher of fp1*/
void FrameProcessor::matchKeypoints(FrameProcessor fp1, FrameProcessor fp2, std::vector<cv::DMatch> &matches) {
	
	// using OpenCV BRUTEFORCE matcher with knn method to compute the 2 best matches for each keypoint of fp1
	std::vector<std::vector<cv::DMatch>> best_matches;

	if(fp1.descriptors.empty() or fp2.descriptors.empty()) { return; }
	// else
	fp1.matcher -> knnMatch(fp1.descriptors, fp2.descriptors, best_matches, 2);
	// best_matches[i] contains the 2 best matches for fp1 i-th descriptor


	// filtering and adding matches to matches vector
	for (int i=0; i<best_matches.size(); i++) {
		if (best_matches[i][0].distance <  0.75 * best_matches[i][1].distance){
			
			// If the 1st match is significantly (25%) better than the 2nd
			matches.push_back(best_matches[i][0]);

		}	
	}

}

/*Computes the translation that must be applied to the camera to get fp2 frame from fp1 frame. dx and dz are expressed in pixels.
dx corresponds to the camera's horizontal axis, from left to right and dy to the camera's vertical axis from top to bottom.
dz is given in red channel intensity (between 0 and 255). If dz > 0, the camera got closer to the object in front (higher red intensity)
Assumes change of attitude to be negligeable.*/
void FrameProcessor::computeTranslation(FrameProcessor fp1, FrameProcessor fp2, 
										float &dx, float &dy, float &dz) {

	// matching keypoints
	std::vector<cv::DMatch> matches;
	FrameProcessor::matchKeypoints(fp1, fp2, matches);

	// computing x, y translation by looking at median translation for each keypoint
	std::vector<float> dxs, dys;
	for (auto match = begin(matches); match != end(matches); ++match) {
		dxs.push_back( fp2.keypoints[match -> trainIdx].pt.x - fp1.keypoints[match -> queryIdx].pt.x );
		dys.push_back( fp2.keypoints[match -> trainIdx].pt.y - fp1.keypoints[match -> queryIdx].pt.y );
	}
	if (dxs.empty()) {return;}
	if (dys.empty()) {return;}

	dx = median(dxs); dy = median(dys);


	// computing z translation by looking at median red channel variation for each keypoint
	std::vector<float> dzs;
	for (auto match=begin(matches); match != end(matches); ++match) {
		dzs.push_back( fp2.keypoints_red_component[match -> trainIdx] - fp1.keypoints_red_component[match -> queryIdx] );
	}
	if (dzs.empty()) return;
	dz = median(dzs);
}

void FrameProcessor::frame_safeLock() {
	if (frameCopied) {
		return;
	} //else
	frame = new cv::Mat(frame -> clone());
	frameCopied = true;
}

size_t FrameProcessor::get_numberOfKeypoints() { return descriptors.total(); }

void FrameProcessor::get_frame(cv::Mat &output_frame) {
	if (frameCopied) {
		frame->copyTo(output_frame);
	} else {
		std::cerr << "Frame not locked, cannot copy. Abort getting frame" << std::endl;
	}
}