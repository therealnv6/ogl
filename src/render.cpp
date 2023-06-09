#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <render.hpp>

namespace gfx
{
	void enable(uint16_t flag)
	{
		glEnable(flag);
	}

	void depth(uint16_t flag)
	{
		if (!glIsEnabled(GL_DEPTH_TEST))
		{
			gfx::enable(gfx::enable_fields::DepthTest);
		}

		glDepthFunc(flag);
	}

	void clear(uint16_t buffers)
	{
		glClear(static_cast<uint16_t>(buffers));
	}

	void clear_color(std::array<float, 4> color)
	{
		glClearColor(color[0], color[1], color[2], color[3]);
	}

	void enable_vertex(int attribute_index)
	{
		glEnableVertexAttribArray(attribute_index);
	}

	void vertex_attribute(int attributeIndex, int size, void *offset)
	{
		glVertexAttribPointer(
			attributeIndex, // attribute index
			size, // size
			GL_FLOAT, // type
			GL_FALSE, // normalized
			0, // stride
			offset // array buffer offset
		);
	}

	void draw_arrays(int attributeIndex, int count)
	{
		glDrawArrays(GL_TRIANGLES, 0, count);
	}

	void draw_elements(int indices)
	{
		glDrawElements(GL_TRIANGLES, indices, GL_UNSIGNED_INT, (void *) 0);
	}
}

namespace imgui
{
	void frame()
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
			}
		}
	}
}
