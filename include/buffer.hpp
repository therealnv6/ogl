#pragma once
#include <GL/glew.h>
#include <functional>
#include <memory>
#include <render.hpp>

enum class draw_type : int
{
	static_draw = GL_STATIC_DRAW,
	dynamic_draw = GL_DYNAMIC_DRAW,
	stream_draw = GL_STREAM_DRAW
};

enum class buffer_type : int
{
	array = GL_ARRAY_BUFFER,
	shader_storage = GL_SHADER_STORAGE_BUFFER,
	uniform_buffer = GL_UNIFORM_BUFFER,
};

namespace buffer
{
	class buffer
	{
	public:
		/**
		 * Creates a buffer object and initializes it with the specified data.
		 *
		 * @param data          A pointer to the data that will be stored in the buffer.
		 * @param size          The size (in bytes) of the data to be stored in the buffer.
		 * @param type          The type of drawing operations that will be performed with the buffer.
		 * @param buffer_type   The type of buffer being created.
		 *
		 * @remarks This constructor generates a buffer ID using glGenBuffers and initializes the buffer
		 *          with the specified data using glBufferData. It is recommended to preallocate memory
		 *          using the "size" parameter, as resizing later on is not recommended unless needed.
		 */
		buffer(void *data, int size, draw_type type, buffer_type buffer_type)
			: _draw_type { type }
			, _buffer_type(buffer_type)
			, size { size }
		{
			glGenBuffers(1, &buffer_id);

			this->bind([&]() {
				glBufferData(static_cast<int>(buffer_type), size, data, static_cast<int>(type));
			});
		}

		/**
		 * Updates the data stored in the buffer with new data.
		 *
		 * @param data  A pointer to the new data that will replace the existing data in the buffer.
		 *
		 * @remarks This function uses glBufferSubData to update the buffer with the new data. The size
		 *          of the buffer remains the same.
		 */
		void update(void *data)
		{
			this->bind([&]() {
				glBufferSubData(static_cast<int>(_buffer_type), 0, size, data);
			});
		}

		/**
		 * Resizes the buffer to a new size.
		 *
		 * @param new_size  The new size (in bytes) of the buffer.
		 *
		 * @remarks This function resizes the buffer using glBufferData. If the new size is larger than
		 *          the current size, the buffer will be reallocated. If the new size is smaller, the
		 *          existing data will be truncated. Resizing the buffer should be done sparingly, as it
		 *          can be an expensive operation.
		 */
		void resize(int new_size)
		{
			// if (size <= new_size)
			{
				this->bind([&]() {
					glBufferData(static_cast<int>(_buffer_type), new_size, nullptr, static_cast<int>(_draw_type));
				});
				size = new_size;
			}
		}

		/**
		 * Writes a portion of data into the buffer at the specified offset.
		 *
		 * @param data        A pointer to the data that will be written into the buffer.
		 * @param data_size   The size (in bytes) of the data to be written.
		 * @param offset      The offset (in bytes) at which to write the data in the buffer.
		 *
		 * @remarks This function uses glBufferSubData to write the data into the buffer at the specified
		 *          offset. The existing data at the offset will be overwritten.
		 */
		void write(void *data, int data_size, int offset)
		{
			this->bind([&]() {
				glBufferSubData(static_cast<int>(_buffer_type), offset, data_size, data);
			});
		}

		/**
		 * Binds the buffer and executes the specified callback function.
		 *
		 * @param callback  A function or lambda expression to be executed while the buffer is bound.
		 *
		 * @remarks This function binds the buffer using glBindBuffer, executes the provided callback
		 *          function, and then unbinds the buffer. It ensures that the buffer is bound before
		 *          performing any operations on it.
		 */
		void bind(std::function<void()> callback)
		{
			glBindBuffer(static_cast<int>(_buffer_type), buffer_id);
			callback();
			glBindBuffer(static_cast<int>(_buffer_type), 0);
		}

		/**
		 * Binds the buffer as a vertex attribute for rendering.
		 *
		 * @param attribute_pos  The position of the vertex attribute in the vertex shader.
		 * @param size           The number of components per vertex attribute.
		 * @param offset         An optional offset (in bytes) to apply to the vertex attribute data.
		 *
		 * @remarks This function enables the specified vertex attribute using gfx::enable_vertex,
		 *          binds the buffer, and configures the vertex attribute using gfx::vertex_attribute.
		 *          It is typically used before rendering to specify the vertex data to be used.
		 */
		void bind_vertex(int attribute_pos, int size, void *offset = nullptr)
		{
			gfx::enable_vertex(attribute_pos);

			this->bind([&]() {
				gfx::vertex_attribute(attribute_pos, size, offset);
			});
		}

		/**
		 * Binds the buffer as an index buffer for rendering.
		 *
		 * @remarks This function binds the buffer as the current element array buffer (GL_ELEMENT_ARRAY_BUFFER)
		 *          using glBindBuffer. It is used when rendering indexed primitives.
		 */
		void bind_indices()
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer_id);
		}

		void bind_buffer_base(int *binding)
		{
			glBindBufferBase(GL_UNIFORM_BUFFER, *binding, buffer_id);
		}

		/**
		 * Returns the size (in bytes) of the data stored in the buffer.
		 *
		 * @return The size of the buffer data.
		 */
		int get_size()
		{
			return size;
		}

	private:
		GLuint buffer_id;
		draw_type _draw_type;
		buffer_type _buffer_type;
		int size;
	};

	typedef std::unique_ptr<buffer> unique_buffer;
	typedef std::shared_ptr<buffer> shared_buffer;

	/**
	 * Creates and returns a new vertex array object (VAO) with the specified index.
	 *
	 * @param index  The index of the vertex array object.
	 *
	 * @return The ID of the created vertex array object.
	 *
	 * @remarks This function generates a new vertex array object using glGenVertexArrays and assigns
	 *          the specified index to it. The generated vertex array object can be used to encapsulate
	 *          vertex attribute configurations and vertex buffer bindings.
	 */
	static GLuint reserve_vertex_array(GLuint index = 1)
	{
		GLuint id;

		glGenVertexArrays(index, &id);
		glBindVertexArray(id);

		return id;
	}
}
