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
		GLFWwindow *window; // (In the accompanying source code, this variable is global for simplicity)

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

		void take(std::function<void(GLFWwindow *)> call)
		{
			call(window);
		}

		void swap_buffers()
		{
			glfwSwapBuffers(window);
		}

		void input_mode(input::input_mode mode, GLenum value)
		{
			glfwSetInputMode(window, static_cast<int>(mode), value);
		}

		void poll_events()
		{
			glfwPollEvents();
		}

		int width()
		{
			return size().first;
		}

		int height()
		{
			return size().second;
		}

		std::pair<int, int> size()
		{
			int width;
			int height;

			glfwGetWindowSize(window, &width, &height);

			return std::make_pair(width, height);
		}

		std::pair<double, double> get_mouse_pos()
		{
			double xpos;
			double ypos;

			glfwGetCursorPos(window, &xpos, &ypos);

			return std::make_pair(xpos, ypos);
		}

		void reset_mouse()
		{
			auto [width, height] = this->size();

			glfwSetCursorPos(window,
				width / 2.0,
				height / 2.0);
		}
	};
}
