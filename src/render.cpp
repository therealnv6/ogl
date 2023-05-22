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
}
