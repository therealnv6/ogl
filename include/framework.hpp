#pragma once
#include <input.hpp>
#include <window.hpp>

namespace frame
{
	class framework
	{
	public:
		float lastTime = glfwGetTime();
		float deltaTime;

		gfx::context *context;

		framework(gfx::context *context)
		{
			this->context = context;
		}

		virtual void initialize() = 0;
		virtual void tick(GLFWwindow *window, framework &framework) = 0;

		void run()
		{
			context->take([&](auto window) {
				do
				{
					double currentTime = glfwGetTime();
					deltaTime = float(currentTime - lastTime);

					tick(window, *this);

					context->swap_buffers();
					context->poll_events();

					lastTime = currentTime;
				} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);
			});
		}

		bool is_pressed(input::input key)
		{
			return glfwGetKey(context->window, static_cast<int>(key)) == GLFW_PRESS;
		}

	private:
	};
}
