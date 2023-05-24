#pragma once
#include <GL/glew.h>
#include <array>

namespace gfx
{
	enum clear_buffer : uint16_t
	{
		Color = 0x00004000,
		Depth = 0x00000100,
		Stencil = 0x00000400,
		ColorDepth = Color | Depth,
		All = Color | Depth | Stencil
	};

	void clear(uint16_t buffers);
	void clear_color(std::array<float, 4> color);
	void enable_vertex(int attribute_index);
	void vertex_attribute(int attributeIndex, int size, void *offset);
	void draw_arrays(int attributeIndex, int count);
}
