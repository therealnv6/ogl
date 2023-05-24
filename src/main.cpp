#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <buffer.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <render.hpp>
#include <spdlog/spdlog.h>
#include <window.hpp>

int main()
{
	gfx::context context("voxel", 1024, 768);

	context.take([&](auto window) {
		context.enable_input_mode(input::input_mode::StickyKeys);
		gfx::clear_color({ 0.0, 0.0, 0.4, 0.0 });

		std::array<float, 9> data = {
			-1.0f, -1.0f, 0.0f, //
			1.0f, -1.0f, 0.0f, //
			0.0f, 1.0f, 0.0f, //
		};

		buffer::reserve_vertex_array(1);
		buffer::buffer<std::array<float, 9>> buffer(&data, sizeof(std::array<float, 9>), buffer::draw_type::Static);

		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float) context.width() / (float) context.height(), 0.1f, 100.0f);

		do
		{
			gfx::clear(gfx::clear_buffer::Color);

			buffer.bind();

			gfx::draw_triangles(0, 3, 3);

			context.swap_buffers();
			context.poll_events();

		} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);
	});
}
