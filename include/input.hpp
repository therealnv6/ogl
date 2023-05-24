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
		a = GLFW_KEY_A,
		b = GLFW_KEY_B,
		c = GLFW_KEY_C,
		d = GLFW_KEY_D,
		e = GLFW_KEY_E,
		f = GLFW_KEY_F,
		g = GLFW_KEY_G,
		h = GLFW_KEY_H,
		i = GLFW_KEY_I,
		j = GLFW_KEY_J,
		k = GLFW_KEY_K,
		l = GLFW_KEY_L,
		m = GLFW_KEY_M,
		n = GLFW_KEY_N,
		o = GLFW_KEY_O,
		p = GLFW_KEY_P,
		q = GLFW_KEY_Q,
		r = GLFW_KEY_R,
		s = GLFW_KEY_S,
		t = GLFW_KEY_T,
		u = GLFW_KEY_U,
		v = GLFW_KEY_V,
		w = GLFW_KEY_W,
		x = GLFW_KEY_X,
		y = GLFW_KEY_Y,
		z = GLFW_KEY_Z,
		zero = GLFW_KEY_0,
		one = GLFW_KEY_1,
		two = GLFW_KEY_2,
		three = GLFW_KEY_3,
		four = GLFW_KEY_4,
		five = GLFW_KEY_5,
		six = GLFW_KEY_6,
		seven = GLFW_KEY_7,
		eight = GLFW_KEY_8,
		nine = GLFW_KEY_9,
		esc = GLFW_KEY_ESCAPE,
		spacebar = GLFW_KEY_SPACE,
		shift = GLFW_KEY_LEFT_SHIFT,
	};
}
