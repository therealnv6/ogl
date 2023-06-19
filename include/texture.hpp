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
	GLuint vao;
	GLuint vbo;

public:
	texture(void *data, dimension dim, texture_format format, float width, float height)
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

		// Create and bind a VAO
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &vbo);

		float vertices[] = {
			0.0f, 0.0f, 0.0f, 0.0f,
			width, 0.0f, 1.0f, 0.0f,
			width, height, 1.0f, 1.0f,
			0.0f, height, 0.0f, 1.0f
		};

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) 0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) (2 * sizeof(float)));

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	~texture()
	{
		glDeleteBuffers(1, &vbo);
		glDeleteVertexArrays(1, &vao);
		glDeleteTextures(1, &id);
	}

	void render_textured_quad(float x, float y, float width, float height)
	{
		glBindTexture(GL_TEXTURE_2D, id);
		glBindVertexArray(vao);
		glDrawArrays(GL_QUADS, 0, 4);
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
};
