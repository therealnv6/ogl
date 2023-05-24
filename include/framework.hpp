#pragma once
#include <input.hpp>
#include <window.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace frame
{
	class framework
	{
	public:
		float lastTime = glfwGetTime();
		float deltaTime;

		gfx::context *context;
		bool imgui = false;

		framework(gfx::context *context)
		{
			this->context = context;
		}

		virtual void initialize() = 0;
		virtual void tick(GLFWwindow *window, framework &framework) = 0;
		virtual void tick_gui(GLFWwindow *window, framework &framework) = 0;

		void init_gui()
		{
			imgui = true;

			ImGui::CreateContext();

			ImGuiIO &io = ImGui::GetIO();
			(void) io;

			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

			ImGui::StyleColorsDark();

			ImGui_ImplGlfw_InitForOpenGL(context->window, true);
			ImGui_ImplOpenGL3_Init("#version 150");
		}

		void gui_frame()
		{
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
		}

		void run()
		{
			context->take([&](auto window) {
				do
				{
					context->poll_events();

					double currentTime = glfwGetTime();
					deltaTime = float(currentTime - lastTime);

					tick(window, *this);

					if (imgui)
					{
						tick_gui(window, *this);

						ImGui::Render();
						ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
					}

					lastTime = currentTime;
					context->swap_buffers();
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
