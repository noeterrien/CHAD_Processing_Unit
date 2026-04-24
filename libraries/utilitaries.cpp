#include <vector>
#include <queue>
#include <algorithm>
#include <opencv2/opencv.hpp>

using namespace std;

inline float median(vector<float> &v)
{
    size_t n = v.size() / 2;
    if ( n%2 == 0) { // cas impair
        nth_element(v.begin(), v.begin()+n+1, v.end());
        return (v[n] + v[n+1])/2;
    } else
    { // cas pair
        nth_element(v.begin(), v.begin()+n, v.end());
        return v[n];
    }
}

/* Draws the vector vector starting from the middle of the frame*/
inline void drawArrowFromOrigin(cv::Mat &frame, cv::Point const& vector) {
    cv::Point origin(frame.size[1]/2, frame.size[0]/2);
	cv::Point arrow_end(frame.size[1]/2 + vector.x, frame.size[0]/2 + vector.y);

	cv::arrowedLine(frame, origin, arrow_end, cv::Scalar(255, 0, 0), 5);	
}
// for convenience
inline void drawArrowFromOrigin(cv::Mat &frame, float const x, float const y) { drawArrowFromOrigin(frame, cv::Point(x,y)); }

