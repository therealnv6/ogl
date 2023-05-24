#pragma once
#include <GL/glew.h>

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
		{
			glGenBuffers(1, &buffer_id);
			glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
			glBufferData(GL_ARRAY_BUFFER, size, reinterpret_cast<const void *>(data), static_cast<int>(type));
		}

		void bind()
		{
			glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
		};

	private:
		GLuint buffer_id;
	};

	GLuint reserve_vertex_array(GLuint index = 1);
}