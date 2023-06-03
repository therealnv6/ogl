#pragma once
#include <GL/glew.h>
#include <functional>
#include <memory>
#include <render.hpp>

enum class draw_type : int
{
	Static = GL_STATIC_DRAW,
	Dynamic = GL_DYNAMIC_DRAW,
	Stream = GL_STREAM_DRAW
};

namespace buffer
{
	class buffer
	{
	public:
		buffer(void *data, int size, draw_type type)
			: type { type }
			, size { size }
		{
			glGenBuffers(1, &buffer_id);

			this->bind([&]() {
				glBufferData(GL_ARRAY_BUFFER, size, data, static_cast<int>(type));
			});
		}

		void update(void *data)
		{
			this->bind([&]() {
				glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
			});
		}

		void bind(std::function<void()> callback)
		{
			glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
			callback();
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		void bind_vertex(int attribute_pos, int size, void *offset = nullptr)
		{
			gfx::enable_vertex(attribute_pos);

			this->bind([&]() {
				gfx::vertex_attribute(attribute_pos, size, offset);
			});
		}

	private:
		GLuint buffer_id;
		draw_type type;
		int size;
	};

	typedef std::unique_ptr<buffer> unique_buffer;

	GLuint reserve_vertex_array(GLuint index = 1);
}
