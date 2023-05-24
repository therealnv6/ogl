#include <render.hpp>

namespace gfx
{
	void clear(clear_buffer buffers)
	{
		glClear(static_cast<unsigned int>(buffers));
	}

	void clear_color(std::array<float, 4> color)
	{
		glClearColor(color[0], color[1], color[2], color[3]);
	}

	void draw_triangles(int attributeIndex, int size, int count)
	{
		glEnableVertexAttribArray(attributeIndex);
		glVertexAttribPointer(
			attributeIndex, // attribute index
			size, // size
			GL_FLOAT, // type
			GL_FALSE, // normalized
			0, // stride
			nullptr // array buffer offset
		);
		glDrawArrays(GL_TRIANGLES, 0, count);
		glDisableVertexAttribArray(attributeIndex);
	}
}
