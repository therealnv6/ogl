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

	enum depth_function : uint16_t
	{
		Less = GL_LESS,
		Greater = GL_GREATER,
	};

	enum enable_fields : uint16_t
	{
		CullFace = GL_CULL_FACE,
		DepthTest = GL_DEPTH_TEST,
	};

	void depth(uint16_t flag);
	void enable(uint16_t flags);
	void clear(uint16_t buffers);
	void clear_color(std::array<float, 4> color);
	void enable_vertex(int attribute_index);
	void vertex_attribute(int attributeIndex, int size, void *offset);
	void index_buffer();
	void draw_elements(int indices);
	void draw_arrays(int attributeIndex, int count);
}

namespace imgui
{
	void frame();
}
