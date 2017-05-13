#ifndef _OPENCV_APP_H_
#define _OPENCV_APP_H_

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

static const int	image_width	= 64;
static const int	image_height	= 64;
static const int	image_channels	= 4;
static cv::Mat		image_cv;
static const char*	image_cv_name = "image_opencv";



static void create_cv_image()
{
	image_cv = cv::Mat(image_height, image_width, CV_8UC3, cv::Scalar(0, 255, 0));
}

static void show_cv_image()
{
	cv::namedWindow(image_cv_name, cv::WINDOW_AUTOSIZE);
	cv::imshow(image_cv_name, image_cv);
}


#endif // !_OPENCV_APP_H_