#pragma once
#include <GL/glew.h>

#define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
#define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049

enum class memory_type
{
	available_memory = GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX,
	free_space = GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX,
};

enum class memory_scale
{
	kilobytes,
	megabytes,
	gigabytes
};

namespace info
{
	static GLint get_memory(memory_type type)
	{
		int memory = 0;
		glGetIntegerv(static_cast<int>(type), &memory);

		return memory;
	}

	static float convert_to_scale(GLint memory, memory_scale scale)
	{
		float converted_memory = static_cast<float>(memory);
		switch (scale)
		{
		case memory_scale::gigabytes:
			converted_memory /= 1000.0f;
			[[fallthrough]];
		case memory_scale::megabytes:
			converted_memory /= 1024.0f;
			[[fallthrough]];
		case memory_scale::kilobytes:
			break;
		}
		return converted_memory;
	}

	static float get_memory(memory_type type, memory_scale scale)
	{
		return convert_to_scale(get_memory(type), scale);
	}
};
