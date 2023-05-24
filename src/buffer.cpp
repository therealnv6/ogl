#include <buffer.hpp>

namespace buffer
{
	GLuint reserve_vertex_array(GLuint index)
	{
		GLuint id;

		glGenVertexArrays(index, &id);
		glBindVertexArray(id);

		return id;
	}
}
