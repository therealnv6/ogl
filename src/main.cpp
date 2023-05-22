#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <render.hpp>
#include <spdlog/spdlog.h>
#include <window.hpp>

int main()
{
	gfx::context context("voxel", 1024, 768);

	context.run([&](auto window) {
		context.enable_input_mode(input::input_mode::StickyKeys);
		gfx::clear_color({ 0.0, 0.0, 0.4, 0.0 });

		GLuint VertexArrayID;
		glGenVertexArrays(1, &VertexArrayID);
		glBindVertexArray(VertexArrayID);

		// An array of 3 vectors which represents 3 vertices
		static const GLfloat g_vertex_buffer_data[] = {
			-1.0f,
			-1.0f,
			0.0f,
			1.0f,
			-1.0f,
			0.0f,
			0.0f,
			1.0f,
			0.0f,
		};

		// This will identify our vertex buffer
		GLuint vertexbuffer;
		// Generate 1 buffer, put the resulting identifier in vertexbuffer
		glGenBuffers(1, &vertexbuffer);
		// The following commands will talk about our 'vertexbuffer' buffer
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		// Give our vertices to OpenGL.
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
		// 1st attribute buffer : vertices
		do
		{
			gfx::clear(gfx::clear_buffer::Color);

			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
			glVertexAttribPointer(
				0, // attribute 0. No particular reason for 0, but must match the layout in the shader.
				3, // size
				GL_FLOAT, // type
				GL_FALSE, // normalized?
				0, // stride
				(void *) 0 // array buffer offset
			);
			// Draw the triangle !
			glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
			glDisableVertexAttribArray(0);

			context.swap_buffers();
			context.poll_events();

		} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);
	});
}
