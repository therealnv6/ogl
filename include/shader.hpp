#pragma once
#include <GL/glew.h>
#include <format>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace shader
{
	enum class shader_type
	{
		Vertex = GL_VERTEX_SHADER,
		Fragment = GL_FRAGMENT_SHADER,
		Compute = GL_COMPUTE_SHADER
	};

	class shader
	{
	public:
		shader(const char *vertex_file_path, const char *fragment_file_path)
		{
			id = glCreateProgram();

			this->compile(vertex_file_path, shader_type::Vertex);
			this->compile(fragment_file_path, shader_type::Fragment);
			this->link();
		}

		shader(const char *compute_file_path)
		{
			id = glCreateProgram();

			this->compile(compute_file_path, shader_type::Compute);
			this->link();
		}

		GLuint get_uniform_location(const char *name)
		{
			if (this->uniform_map.contains(name))
			{
				return uniform_map[name];
			}

			return glGetUniformLocation(id, name);
		}

		void bind_mat4(const char *name, const glm::mat4 &matrix, bool transpose)
		{
			auto location = get_uniform_location(name);
			auto ptr = glm::value_ptr(matrix);

			glUniformMatrix4fv(location, 1, transpose, ptr);
		}

		void bind()
		{
			glUseProgram(id);
		}

		~shader()
		{
			glDeleteProgram(id);
		}

		void dispatch_compute(int local_x, int local_y, int local_z)
		{
			this->bind();
			glDispatchCompute(local_x, local_y, local_z);
			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		}

		void set_uniform_buffer(const std::string &name, GLuint bufferBinding)
		{
			GLuint blockIndex = glGetUniformBlockIndex(id, name.c_str());
			glUniformBlockBinding(id, blockIndex, bufferBinding);
			glBindBufferBase(GL_UNIFORM_BUFFER, bufferBinding, bufferBinding);
		}

		template<typename T>
		void set_uniform(const std::string &name, const T &value)
		{
			GLuint program = id;
			GLint location = glGetUniformLocation(program, name.c_str());

			if (location != -1)
			{
				if constexpr (std::is_same_v<T, int>)
				{
					glProgramUniform1i(program, location, value);
				}
				else if constexpr (std::is_same_v<T, float>)
				{
					glProgramUniform1f(program, location, value);
				}
				else if constexpr (std::is_same_v<T, glm::vec3>)
				{
					glProgramUniform3fv(program, location, 1, glm::value_ptr(value));
				}
			}
		}

	private:
		GLuint id;
		std::map<const char *, GLuint> uniform_map;

		void compile(const char *file_path, shader_type type)
		{
			GLuint shader_id = glCreateShader(static_cast<int>(type));

			// Read the shader code from the file
			std::string code;
			std::ifstream stream(file_path, std::ios::in);
			if (stream.is_open())
			{
				std::stringstream sstr;
				sstr << stream.rdbuf();
				code = sstr.str();
				stream.close();
			}
			else
			{
				std::cout << "Failed to open shader file: " << file_path << std::endl;
				return;
			}

			GLint result = GL_FALSE;
			int info_length;

			// Compile the shader
			char const *source_ptr = code.c_str();
			glShaderSource(shader_id, 1, &source_ptr, NULL);
			glCompileShader(shader_id);

			// Check for compilation errors
			glGetShaderiv(shader_id, GL_COMPILE_STATUS, &result);
			glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &info_length);
			if (info_length > 0)
			{
				std::vector<char> error_msg(info_length + 1);
				glGetShaderInfoLog(shader_id, info_length, NULL, &error_msg[0]);
				throw std::runtime_error(
					std::format("shader compilation error {}", &error_msg[0]) // ugly, but deal with it.
				);
			}

			glAttachShader(id, shader_id);
		}

		void link()
		{
			GLint result = GL_FALSE;
			int log_length;

			// Link the program
			glLinkProgram(id);

			// Check the program
			glGetProgramiv(id, GL_LINK_STATUS, &result);
			glGetProgramiv(id, GL_INFO_LOG_LENGTH, &log_length);
			if (log_length > 0)
			{
				std::vector<char> error_msg(log_length + 1);
				glGetProgramInfoLog(id, log_length, NULL, &error_msg[0]);

				throw std::runtime_error("program linking error");
				return;
			}
		}
	};
}
