#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <spdlog/spdlog.h>

namespace input
{
	enum class input_mode
	{
		Cursor = GLFW_CURSOR,
		StickyKeys = GLFW_STICKY_KEYS,
		StickyMouseButtons = GLFW_STICKY_MOUSE_BUTTONS,
		LockKeyMods = GLFW_LOCK_KEY_MODS,
		RawMouseMotion = GLFW_RAW_MOUSE_MOTION
	};

	enum class input
	{
		w = GLFW_KEY_W,
		a = GLFW_KEY_A,
		s = GLFW_KEY_S,
		d = GLFW_KEY_D,
	};
}
