
#include "time.h"
#include "glfwapp.h"
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

static void test_cases();


static cv::Mat get_cv_img_from_gl_img(GLuint ogl_texture_id)
{
	glBindTexture(GL_TEXTURE_2D, ogl_texture_id);
	GLint gl_texture_width, gl_texture_height;

	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &gl_texture_width);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &gl_texture_height);

	unsigned char* gl_texture_bytes = (unsigned char*)malloc(sizeof(unsigned char) * gl_texture_width * gl_texture_height * 4);
	glGetTexImage(GL_TEXTURE_2D, 0, pixel_format, GL_UNSIGNED_BYTE, gl_texture_bytes);

	return cv::Mat(gl_texture_height, gl_texture_width, CV_8UC4, gl_texture_bytes);
}



static cv::Mat get_cv_img_from_gl_buffer(GLuint ogl_buffer_id, GLuint ogl_texture_id)
{
	cv::Mat cv_img;

	glBindBuffer(GL_PIXEL_PACK_BUFFER, ogl_buffer_id);
	glBindTexture(GL_TEXTURE_2D, ogl_texture_id);
	
	GLint gl_texture_width, gl_texture_height;
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &gl_texture_width);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &gl_texture_height);

	GLubyte* gl_buffer_bytes = (GLubyte*)glMapBufferARB(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
	if (gl_buffer_bytes)
	{
		cv_img = cv::Mat(gl_texture_height, gl_texture_width, CV_8UC4, gl_buffer_bytes);
		glUnmapBuffer(GL_PIXEL_PACK_BUFFER);     // release pointer to the mapped buffer
	}
	else
	{
		cv_img = cv::Mat(2, 2, CV_8UC4, cvScalar(0, 0, 0, 255));
	}

	gl_buffer_bytes = nullptr;
	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
	return cv_img;
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
	// Transfer texture to PBO
	beginTime();
	transfer_to_pbo(texture_id);
	endTime("transfer texture to pbo (ms)   : ");
	
	//
	// map/ummap buffer from glTexture
	//
	beginTime();
	cv::Mat cv_gl_pbo_buf = get_cv_img_from_gl_buffer(pbo_id, texture_id);
	endTime("cv from glMap/glUnmap PBO (ms) : ");
	
	//
	// glGetTexImage from glTexture
	//
	beginTime();
	cv::Mat cv_gl_image = get_cv_img_from_gl_img(texture_id);
	endTime("cv from glGetTexImage TEX (ms) : ");

	


	//
	// OpenCV windows
	//
	const char* window_tex = "cv_gl_texture";
	const char* window_pbo_buf = "cv_gl_pbo_buf";
	cv::namedWindow(window_tex);
	cv::namedWindow(window_pbo_buf);
	cv::imshow(window_tex, cv_gl_image);
	cv::imshow(window_pbo_buf, cv_gl_pbo_buf);


	//test_cases();

	// 
	// GL loop
	//
	main_loop();
	

	//
	// Destroy OpenCV windows
	//
	cv::destroyWindow(window_tex);
	cv::destroyWindow(window_pbo_buf);
	

	// 
	//	Cleanup OpenGL 
	//
	cleanup();


	return EXIT_SUCCESS;
}





static void test_cases()
{
	std::cout 
		<< std::fixed << std::endl
		<< "running test cases..." << std::endl;

	int iterations = 1000;
	double total_time[2] = { 0, 0 };

	//
	// test case 0
	//
	total_time[0] = total_time[1] = 0;
	for (int i = 0; i < iterations; ++i)
	{
		beginTime();
		get_cv_img_from_gl_buffer(pbo_id, texture_id);
		total_time[0] += diffTime();;

		beginTime();
		get_cv_img_from_gl_img(texture_id);
		total_time[1] += diffTime();;
	}
	std::cout << "mean time case 0 : " << total_time[0] / iterations << ' ' << total_time[1] / iterations << std::endl;

	//
	// test case 1
	//
	total_time[0] = total_time[1] = 0;
	for (int i = 0; i < iterations; ++i)
	{
		beginTime();
		get_cv_img_from_gl_img(texture_id);
		total_time[1] += diffTime();;

		beginTime();
		get_cv_img_from_gl_buffer(pbo_id, texture_id);
		total_time[0] += diffTime();;

	}
	std::cout << "mean time case 1 : " << total_time[0] / iterations << ' ' << total_time[1] / iterations << std::endl;


	//
	// test case 1
	//
	total_time[0] = total_time[1] = 0;
	for (int i = 0; i < iterations; ++i)
	{
		beginTime();
		get_cv_img_from_gl_img(texture_id);
		total_time[1] += diffTime();;

	}

	for (int i = 0; i < iterations; ++i)
	{
		beginTime();
		get_cv_img_from_gl_buffer(pbo_id, texture_id);
		total_time[0] += diffTime();;

	}
	std::cout << "mean time case 2 : " << total_time[0] / iterations << ' ' << total_time[1] / iterations << std::endl;
}
