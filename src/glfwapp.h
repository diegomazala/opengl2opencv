#ifndef _GLFW_APP_H_
#define _GLFW_APP_H_




#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <FreeImage/FreeImage.h>
#include <iostream>
#include <list>
#include <memory>
#include <string>


const char* tex_vertex_shader =
"#version 330 core \n"
"layout(location = 0) in vec3 position; \n"
"layout(location = 1) in vec3 color; \n"
"layout(location = 2) in vec2 texCoord; \n"
"out vec3 vColor; \n"
"out vec2 TexCoord; \n"
"void main() \n"
"{ \n"
"	gl_Position = vec4(position, 1.0f); \n"
"	vColor = color; \n"
"	TexCoord = texCoord; \n"
"} \n";

const char* tex_fragment_shader =
"#version 330 core \n"
"in vec3 vColor; \n"
"in vec2 TexCoord; \n"
"out vec4 outColor; \n"
"uniform sampler2D MainTexture; \n"
"void main() \n"
"{ \n"
"	outColor = texture(MainTexture, TexCoord); \n"
"} \n";


static GLFWwindow* window;
static GLuint shader_program;
static GLuint vbo, ebo;
static GLuint triangle_vao;
static GLuint quad_vao;

static GLuint texture_id;
static GLuint pbo_id;

static const GLenum pixel_format = GL_RGBA;

static const int tex_width = 256;
static const int tex_height = 256;
static const int tex_channels = 4;
static GLubyte texture_data[tex_width][tex_height][tex_channels];

static const int buffer_width = tex_width;
static const int buffer_height = tex_height;
static const int buffer_channels = tex_channels;
static const int buffer_data_size = buffer_width * buffer_height * buffer_channels;

static float red_channel = 0.2f;
static float color_inc = 0.01f;

#define check_gl_error() _check_gl_error(__FILE__,__LINE__)
void _check_gl_error(const char *file, int line) 
{
	GLenum err(glGetError());

	while (err != GL_NO_ERROR) 
	{
		std::string error;
		switch (err) 
		{
			case GL_INVALID_OPERATION:      error = "INVALID_OPERATION";    break;
			case GL_INVALID_ENUM:           error = "INVALID_ENUM";         break;
			case GL_INVALID_VALUE:          error = "INVALID_VALUE";        break;
			case GL_OUT_OF_MEMORY:          error = "OUT_OF_MEMORY";        break;
			case GL_STACK_OVERFLOW:			error = "GL_STACK_OVERFLOW";	break;
			case GL_STACK_UNDERFLOW:		error = "GL_STACK_UNDERFLOW";	break;
			case GL_CONTEXT_LOST:			error = "GL_CONTEXT_LOST";		break;
			case GL_TABLE_TOO_LARGE:		error = "GL_TABLE_TOO_LARGE1";	break;
			case GL_INVALID_FRAMEBUFFER_OPERATION:  error = "INVALID_FRAMEBUFFER_OPERATION";  break;
			default:						error = "GL ERROR UNKNOWN";	break;
		}
		std::cerr << "GL_" << error.c_str() << " - " << file << ":" << line << std::endl;
		err = glGetError();
	}
}

static void bakeShader()
{
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &tex_vertex_shader, NULL);
	glCompileShader(vs);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &tex_fragment_shader, NULL);
	glCompileShader(fs);

	shader_program = glCreateProgram();
	glAttachShader(shader_program, fs);
	glAttachShader(shader_program, vs);
	glLinkProgram(shader_program);
}


static void buildTriangle()
{
	float points[] = {
		0.0f,  0.5f,  0.0f,
		0.5f, -0.5f,  0.0f,
		-0.5f, -0.5f,  0.0f
	};

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), points, GL_STATIC_DRAW);

	triangle_vao = 0;
	glGenVertexArrays(1, &triangle_vao);
	glBindVertexArray(triangle_vao);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
}



static void buildQuad()
{
	// Set up vertex data (and buffer(s)) and attribute pointers
	GLfloat vertices[] = {
		// Positions          // Colors           // Texture Coords
		0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // Top Right
		0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // Bottom Right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // Bottom Left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // Top Left 
	};
	GLuint indices[] = {  // Note that we start from 0!
		0, 1, 3, // First Triangle
		1, 2, 3  // Second Triangle
	};
	
	glGenVertexArrays(1, &quad_vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(quad_vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// TexCoord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO
}


void create_pbo()
{
	// create 2 pixel buffer objects, you need to delete them when program exits.
	// glBufferDataARB with NULL pointer reserves only memory space.
	glGenBuffers(1, &pbo_id);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo_id);
	glBufferData(GL_PIXEL_PACK_BUFFER, buffer_data_size, 0, GL_STREAM_READ_ARB);
	glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, 0);
}


void transfer_to_pbo(GLuint tex_id)
{
	// bind PBO
	glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo_id);

	// bind texture
	glBindTexture(GL_TEXTURE_2D, tex_id);

	// transfer texture into PBO
	glGetTexImage(GL_TEXTURE_2D,
		0,
		GL_BGRA,
		GL_UNSIGNED_BYTE,
		(GLvoid*)0 // offset in bytes into "buffer",
				   // not pointer to client memory!
	);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
}



static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}



static void render()
{
	// Make the window's context current
	glfwMakeContextCurrent(window);


	if (red_channel > 1.0f || red_channel < 0.0f)
	{
		color_inc *= -1.f;
		red_channel = (int)red_channel;
	}

	red_channel += color_inc;

	glClearColor(red_channel, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shader_program);

	// Bind Textures using texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glUniform1i(glGetUniformLocation(shader_program, "MainTexture"), 0);

	// Draw container
	glBindVertexArray(quad_vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// Swap front and back buffers
	glfwSwapBuffers(window);
}



static int create_window()
{
	// Initialise GLFW
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);								// 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);					// We want OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);			// To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	//We don't want the old OpenGL 

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(256, 256, "OpenGL Window", NULL, NULL);
	if (window == NULL) 
	{
		std::cerr << "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials." << std::endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glfwSetWindowPos(window, 0, 30);

	glfwMakeContextCurrent(window); // Initialize GLEW
	glewExperimental = true; // Needed in core profile
	if (glewInit() != GLEW_OK) 
	{
		std::cerr << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	glfwSetKeyCallback(window, key_callback);

	return EXIT_SUCCESS;
}



static void main_loop()
{
	glfwFocusWindow(window);

	while (!glfwWindowShouldClose(window))
	{
		// Poll for and process events
		glfwPollEvents();

		render();
	}
}



static void cleanup()
{
	// Properly de-allocate all resources once they've outlived their purpose
	glDeleteVertexArrays(1, &quad_vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
}



static float time_ticks()
{
	typedef std::chrono::high_resolution_clock clock;
	typedef std::chrono::duration<float, std::milli> duration;

	static clock::time_point start = clock::now();
	duration elapsed = clock::now() - start;
	return elapsed.count();
}

static void fill_texture_data()
{
	const float PI = 3.141592f;

	for (int i = 0; i < tex_height; i++)
	{
		for (int j = 0; j < tex_width; j++)
		{
#if 0
			int c = ((((i & 0x8) == 0) ^ ((j & 0x8)) == 0)) * 255;
			texture_data[i][j][0] = (GLubyte)c;
			texture_data[i][j][1] = (GLubyte)c;
			texture_data[i][j][2] = (GLubyte)c;
			texture_data[i][j][3] = (GLubyte)255;
#else
			float uv_x = float(j + 1) / float(tex_width);
			float uv_y = float(i + 1) / float(tex_width);
			float pos_x = PI * (2.f * uv_x - 1.f);
			float pos_y = PI * (2.f * uv_y - 1.f);
			float ch = abs(sin(10.f * pos_y + 10.f * sin(pos_x + time_ticks() * 0.002f)));

			texture_data[i][j][0] = (GLubyte)(50 * ch);
			texture_data[i][j][1] = (GLubyte)(130 * ch);
			texture_data[i][j][2] = (GLubyte)(255 * ch);
			texture_data[i][j][3] = (GLubyte)255;
#endif
		}
	}
}

static void create_texture()
{
	fill_texture_data();

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, pixel_format, tex_width, tex_height, 0, pixel_format, GL_UNSIGNED_BYTE, texture_data);
}



#endif // !_GLFW_APP_H_