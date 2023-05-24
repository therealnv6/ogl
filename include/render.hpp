#pragma once
#include <GL/glew.h>
#include <array>

namespace gfx
{
	enum class clear_buffer
	{
		Color = 0x00004000,
		Depth = 0x00000100,
		Stencil = 0x00000400,
		ColorDepth = Color | Depth,
		All = Color | Depth | Stencil
	};

	void clear(clear_buffer buffers);
	void clear_color(std::array<float, 4> color);
	void draw_triangles(int attributeIndex, int size, int count);
}
