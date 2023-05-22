#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <array>
#include <glm/glm.hpp>
#include <input.hpp>
#include <iostream>
#include <spdlog/spdlog.h>

namespace gfx
{
	class context
	{
	public:
		context(const char *title, uint16_t width, uint16_t height)
		{
			if (!glfwInit())
			{
				throw std::runtime_error("unable to initialize glfw");
			}

			glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
			glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL

			window = glfwCreateWindow(width, height, title, NULL, NULL);

			if (window == NULL)
			{
				glfwTerminate();
				throw std::runtime_error("failed to open glfw window");
			}

			glfwMakeContextCurrent(window); // Initialize GLEW
			glewExperimental = true;

			if (glewInit() != GLEW_OK)
			{
				throw std::runtime_error("unable to initialize glew");
			}
		};

		void run(std::function<void(GLFWwindow *)> call)
		{
			call(window);
		}

		void swap_buffers()
		{
			glfwSwapBuffers(window);
		}

		void enable_input_mode(input::input_mode mode)
		{
			glfwSetInputMode(window, static_cast<int>(mode), GLFW_TRUE);
		}

		void disable_input_mode(input::input_mode mode)
		{
			glfwSetInputMode(window, static_cast<int>(mode), GLFW_FALSE);
		}

		void poll_events()
		{
			glfwPollEvents();
		}

	private:
		GLFWwindow *window; // (In the accompanying source code, this variable is global for simplicity)
	};
}
