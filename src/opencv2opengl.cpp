
#include "time.h"
#include "glfwapp.h"
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>



static float mticks()
{
	typedef std::chrono::high_resolution_clock clock;
	typedef std::chrono::duration<float, std::milli> duration;

	static clock::time_point start = clock::now();
	duration elapsed = clock::now() - start;
	return elapsed.count();
}

static void fill_cv_image(cv::Mat& cv_image)
{
	const float PI = 3.141592f;

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


void get_gl_tex_from_cv_img(GLuint tex_id, const cv::Mat& image)
{
	if (image.empty())
	{
		std::cerr << "Error: image empty" << std::endl;
		return;
	}

	glBindTexture(GL_TEXTURE_2D, tex_id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Set texture clamping method
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexImage2D(
		GL_TEXTURE_2D,       // Type of texture
		0,                   // Pyramid level (for mip-mapping) - 0 is the top level
		GL_RGB,              // Internal colour format to convert to
		image.cols,          // Image width  
		image.rows,          // Image height 
		0,                   // Border width in pixels (can either be 1 or 0)
		GL_BGR,              // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
		GL_UNSIGNED_BYTE,    // Image data type
		image.ptr());        // The actual image data itself

	glBindTexture(GL_TEXTURE_2D, 0);
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
	cv::Mat cv_image = cv::Mat(tex_width, tex_height, CV_8UC3, cv::Scalar(0, 0, 0));
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

		beginTime();
		fill_cv_image(cv_image);
		endTime("cv fill image (ms)             : ");

		cv::imshow(cv_window, cv_image);

		beginTime();
		get_gl_tex_from_cv_img(texture_id, cv_image);
		endTime("gl tex from cv img (ms)        : ");
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




