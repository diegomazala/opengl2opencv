
#include "time.h"
#include "glfwapp.h"
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

const float PI = 3.141592f;

float mticks()
{
	typedef std::chrono::high_resolution_clock clock;
	typedef std::chrono::duration<float, std::milli> duration;

	static clock::time_point start = clock::now();
	duration elapsed = clock::now() - start;
	return elapsed.count();
}

static float t = 0;
void fill_cv_image(cv::Mat& cv_image)
{
	for (int i = 0; i<cv_image.rows; i++)
	{
		for (int j = 0; j<cv_image.cols; j++)
		{
			cv::Vec2f uv(float(j + 1) / float(cv_image.cols), float(i + 1) / float(cv_image.rows));
			cv::Vec2f pos(PI * (2.f * uv[0] - 1.f), PI * (2.f * uv[1] - 1.f));
			float ch = abs(sin(10.f * pos[1] + 10.f * sin(pos[0] + mticks() * 0.002f)));
			cv_image.at<cv::Vec3b>(i, j) = cv::Vec3b(255 * ch, 130 * ch, 50 * ch);
		}
	}
}

int main(void)
{	
	// 
	//	Setup OpenGL 
	//
	create_window();
	create_texture();
	create_pbo();
	bakeShader();
	buildQuad();
	

	//
	// OpenCV windows
	//
	const char* cv_window = "cv_window";
	cv::Mat cv_image = cv::Mat(256, 256, CV_8UC3, cv::Scalar(0, 0, 0));
	cv::namedWindow(cv_window);
	fill_cv_image(cv_image);
	

	
	// 
	// GL loop
	//
	//main_loop();
	glfwFocusWindow(window);
	while (!glfwWindowShouldClose(window))
	{
		// Poll for and process events
		glfwPollEvents();

		render();

		fill_cv_image(cv_image);
		cv::imshow(cv_window, cv_image);
	}

	//
	// Destroy OpenCV windows
	//
	cv::destroyWindow(cv_window);
	

	// 
	//	Cleanup OpenGL 
	//
	cleanup();


	return EXIT_SUCCESS;
}




