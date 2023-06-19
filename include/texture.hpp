#pragma once

#include <GL/glew.h>

enum texture_format
{
	rgba = GL_RGBA
};

enum dimension
{
	d1d = GL_TEXTURE_1D,
	d2d = GL_TEXTURE_2D,
	d3d = GL_TEXTURE_3D,
};

class texture
{
private:
	GLuint id;

public:
	texture(void *data, dimension dim, texture_format format, int width, int height)
	{
		glGenTextures(1, &id);
		glBindTexture(static_cast<int>(dim), id);
		glTexImage2D(static_cast<int>(dim),
			0,
			static_cast<int>(format),
			width,
			height,
			0,
			static_cast<int>(format),
			GL_UNSIGNED_BYTE,
			data);
	}

	~texture()
	{
		glDeleteTextures(1, &id);
	}

	void render_textured_quad(float x, float y, float width, float height)
	{
		// Bind the texture
		glBindTexture(GL_TEXTURE_2D, id);

		// Render the quad
		glBegin(GL_QUADS);
		{
			glTexCoord2f(0, 0);
			glVertex2f(x, y);
			glTexCoord2f(1, 0);
			glVertex2f(x + width, y);
			glTexCoord2f(1, 1);
			glVertex2f(x + width, y + height);
			glTexCoord2f(0, 1);
			glVertex2f(x, y + height);
		}
		glEnd();

		glBindTexture(GL_TEXTURE_2D, 0);
	}
};
