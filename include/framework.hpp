#pragma once
#include <input.hpp>
#include <window.hpp>

#include "GLFW/glfw3.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <entt/entt.hpp>
#include <spdlog/logger.h>

namespace frame
{
	class frame_history
	{
	private:
		int frontIndex; // Index of the front element
		int backIndex; // Index of the back element
		int count; // Current number of frames in the history

	public:
		std::vector<float> frames;
		int max_frames = 1000;

		frame_history()
			: frontIndex(0)
			, backIndex(0)
			, count(0)
			, frames(1000, 0.0f)
		{
		}

		void push(float frame)
		{
			frames[backIndex] = frame;
			backIndex = (backIndex + 1) % max_frames;

			if (count == max_frames)
			{
				frontIndex = (frontIndex + 1) % max_frames;
			}
			else
			{
				count++;
			}
		}

		void pop()
		{
			if (count > 0)
			{
				frontIndex = (frontIndex + 1) % frames.size();
				count--;
			}
		}

		void resize()
		{
			if (max_frames == this->frames.size())
			{
				return;
			}

			int new_max_frames = max_frames;
			std::vector<float> new_frames(new_max_frames, 0.0f);

			int num_frames = std::min(count, new_max_frames);
			for (int i = 0; i < num_frames; i++)
			{
				new_frames[i] = frames[(frontIndex + i) % max_frames];
			}

			frames = new_frames;
			frontIndex = 0;
			backIndex = num_frames;
			count = num_frames;

			max_frames = new_max_frames;
		}
	};

	class framework;

	struct time {
		float lastTime;
		float deltaTime;
		frame_history frameHistory;
	};

	struct tick_event {
		time value;
		frame::framework *data;
		entt::registry *registry;
	};

	class framework
	{
	public:
		gfx::context *context;

		entt::dispatcher &dispatcher;
		entt::registry &registry;

		bool imgui = false;
		time frame;

		framework(gfx::context *context, entt::registry &registry, entt::dispatcher &dispatcher)
			: registry(registry)
			, dispatcher(dispatcher)
		{
			this->context = context;
			registry.emplace<time>(this->registry.create(), glfwGetTime(), 0.0f);
		}

		void init_gui()
		{
			imgui = true;

			ImGui::CreateContext();

			ImGuiIO &io = ImGui::GetIO();
			(void) io;

			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls
			io.MouseDrawCursor = true;

			style_imgui();
			ImGui_ImplGlfw_InitForOpenGL(context->window, true);
			ImGui_ImplOpenGL3_Init("#version 150");
		}

		void gui_frame()
		{
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();

			ImGui::NewFrame();

			ImGuiIO &io = ImGui::GetIO();
			(void) io;

			if (ImGui::GetCurrentContext() == nullptr || !ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow))
			{
				if (io.MouseDrawCursor)
				{
					io.MouseDrawCursor = false;
					context->input_mode(input::input_mode::Cursor, GLFW_CURSOR_DISABLED);
				}
			}

			if (this->is_pressed(input::key::esc))
			{
				context->input_mode(input::input_mode::Cursor, GLFW_CURSOR_HIDDEN);
				ImGui::SetWindowFocus();
				io.MouseDrawCursor = true;
			}
		}

		void run()
		{
			context->take([&](auto window) {
				do
				{
					context->poll_events();

					double currentTime = glfwGetTime();
					frame.deltaTime = float(currentTime - frame.lastTime);

					dispatcher.trigger(tick_event { frame, this, &registry });

					if (imgui)
					{
						ImGuiIO &io = ImGui::GetIO();
						(void) io;

						// for diagnostics

						frame.frameHistory.push(io.Framerate);

						ImGui::Render();
						ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
					}

					frame.lastTime = currentTime;
					context->swap_buffers();
				} while (glfwWindowShouldClose(window) == 0);
			});
		}

		bool is_pressed(input::key key)
		{
			assert(context && context->window);

			return glfwGetKey(context->window, static_cast<int>(key)) == GLFW_PRESS;
		}

	private:
		void style_imgui()
		{
			ImGuiStyle &style = ImGui::GetStyle();

			// Set Style Stuff
			style.FrameRounding = 0.5f;
			style.WindowRounding = 0.5f;
			style.ChildRounding = 0.5f;
			style.PopupRounding = 0.5f;
			style.ScrollbarRounding = 0.5f;
			style.GrabRounding = 0.5f;
			style.TabRounding = 0.5f;

			style.WindowPadding = ImVec2(20.0f, 20.0f);
			style.TouchExtraPadding = ImVec2(10.0f, 10.0f);
			style.FramePadding = ImVec2(5.0f, 5.0f);
			style.ItemInnerSpacing = ImVec2(20.0f, 20.0f);
			style.ItemSpacing = ImVec2(20.0f, 20.0f);
			style.WindowTitleAlign = ImVec2(0.5f, 0.5f);

			style.WindowBorderSize = 0.0f;
			style.FrameBorderSize = 0.0f;
			style.PopupBorderSize = 0.0f;
			style.TabBorderSize = 0.0f;

			style.ScrollbarSize = 20.0f;
			style.IndentSpacing = 30.0f;
			style.GrabMinSize = 20.0f;

			// Set Colors
			ImVec4 *colors = style.Colors;
			colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
			colors[ImGuiCol_TextDisabled] = ImVec4(1.00f, 0.82f, 0.71f, 1.00f);
			colors[ImGuiCol_WindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.67f);
			colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
			colors[ImGuiCol_PopupBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
			colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.30f);
			colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
			colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.20f);
			colors[ImGuiCol_FrameBgHovered] = ImVec4(0.65f, 0.65f, 0.65f, 0.46f);
			colors[ImGuiCol_FrameBgActive] = ImVec4(0.12f, 0.25f, 0.40f, 0.67f);
			colors[ImGuiCol_TitleBg] = ImVec4(1.00f, 0.95f, 0.86f, 0.30f);
			colors[ImGuiCol_TitleBgActive] = ImVec4(0.80f, 0.72f, 0.57f, 1.00f);
			colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.47f, 0.42f, 0.35f, 0.50f);
			colors[ImGuiCol_MenuBarBg] = ImVec4(0.49f, 0.32f, 0.32f, 1.00f);
			colors[ImGuiCol_ScrollbarBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
			colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.54f, 0.70f, 0.50f, 0.80f);
			colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.46f, 0.66f, 0.55f, 0.80f);
			colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.17f, 0.28f, 0.20f, 1.00f);
			colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
			colors[ImGuiCol_SliderGrab] = ImVec4(0.48f, 0.44f, 0.38f, 0.78f);
			colors[ImGuiCol_SliderGrabActive] = ImVec4(0.82f, 0.56f, 0.56f, 0.60f);
			colors[ImGuiCol_Button] = ImVec4(0.81f, 0.67f, 0.83f, 0.40f);
			colors[ImGuiCol_ButtonHovered] = ImVec4(0.79f, 0.25f, 0.79f, 1.00f);
			colors[ImGuiCol_ButtonActive] = ImVec4(0.45f, 0.10f, 0.68f, 1.00f);
			colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
			colors[ImGuiCol_HeaderHovered] = ImVec4(0.69f, 0.62f, 0.45f, 0.80f);
			colors[ImGuiCol_HeaderActive] = ImVec4(0.82f, 0.59f, 0.13f, 1.00f);
			colors[ImGuiCol_Separator] = ImVec4(0.39f, 0.39f, 0.39f, 0.62f);
			colors[ImGuiCol_SeparatorHovered] = ImVec4(0.14f, 0.44f, 0.80f, 0.78f);
			colors[ImGuiCol_SeparatorActive] = ImVec4(0.14f, 0.44f, 0.80f, 1.00f);
			colors[ImGuiCol_ResizeGrip] = ImVec4(0.80f, 0.80f, 0.80f, 0.56f);
			colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
			colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
			colors[ImGuiCol_Tab] = ImVec4(0.84f, 0.76f, 0.76f, 0.93f);
			colors[ImGuiCol_TabHovered] = ImVec4(0.54f, 0.25f, 0.25f, 0.80f);
			colors[ImGuiCol_TabActive] = ImVec4(0.58f, 0.38f, 0.38f, 1.00f);
			colors[ImGuiCol_TabUnfocused] = ImVec4(0.92f, 0.93f, 0.94f, 0.99f);
			colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.74f, 0.82f, 0.91f, 1.00f);
			colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
			colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
			colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
			colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.45f, 0.00f, 1.00f);
			colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
			colors[ImGuiCol_DragDropTarget] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
			colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
			colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.70f, 0.70f, 0.70f, 0.70f);
			colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.20f);
			colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
		}
	};
}
