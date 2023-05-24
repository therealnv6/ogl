#pragma once
#include <GL/glew.h>
#include <render.hpp>

namespace buffer
{
	enum class draw_type
	{
		Static = GL_STATIC_DRAW,
		Dynamic = GL_DYNAMIC_DRAW,
		Stream = GL_STREAM_DRAW
	};

	template<typename T>
	class buffer
	{
	public:
		buffer(T *data, int size, draw_type type)
			: type { type }
			, size { size }
		{
			glGenBuffers(1, &buffer_id);
			glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
			glBufferData(GL_ARRAY_BUFFER, size, reinterpret_cast<const void *>(data), static_cast<int>(type));
		}

		void update(T *data)
		{
			glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
			glBufferSubData(GL_ARRAY_BUFFER, 0, size, reinterpret_cast<const void *>(data));
		}

		void bind()
		{
			glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
		}

		void bind_vertex(int attribute_pos, int size, void *offset = nullptr)
		{
			gfx::enable_vertex(attribute_pos);
			this->bind();
			gfx::vertex_attribute(attribute_pos, size, offset);
		}

	private:
		GLuint buffer_id;
		draw_type type;
		int size;
	};

	GLuint reserve_vertex_array(GLuint index = 1);
}
