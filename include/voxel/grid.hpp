#pragma once
#include "buffer.hpp"
#include "render.hpp"
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <iostream>
#include <spdlog/spdlog.h>
#include <unordered_set>
#include <vector>

namespace voxel
{
	struct voxel_data {
		glm::vec3 color;
		glm::vec3 position;
	};

	struct voxel_data_hash {
		std::size_t operator()(const voxel_data &vd) const
		{
			// Compute a hash based on the position of the voxel_data
			std::size_t h1 = std::hash<float> {}(vd.position.x);
			std::size_t h2 = std::hash<float> {}(vd.position.y);
			std::size_t h3 = std::hash<float> {}(vd.position.z);
			return h1 ^ h2 ^ h3;
		}
	};

	struct voxel_data_equal {
		bool operator()(const voxel_data &vd1, const voxel_data &vd2) const
		{
			return vd1.position == vd2.position;
		}
	};

	typedef std::unordered_set<voxel::voxel_data, voxel::voxel_data_hash, voxel::voxel_data_equal> voxel_set;

	class grid_buffer
	{
	private:
		buffer::buffer *vertex_buffer;
		buffer::buffer *color_buffer;
		buffer::buffer *index_buffer;

	public:
		grid_buffer(glm::vec3 bounds)
		{
			vertex_buffer = new buffer::buffer(nullptr, 0, draw_type::Dynamic);
			color_buffer = new buffer::buffer(nullptr, 0, draw_type::Dynamic);
			index_buffer = new buffer::buffer(nullptr, 0, draw_type::Dynamic);
		}

		void update_buffers(voxel_set data)
		{
			size_t max_buffer_size = data.size();
			size_t type_size = sizeof(glm::vec3);

			spdlog::debug(
				"updating buffers with amount of data: {}, new size is {}, old size is ",
				data.size(),
				max_buffer_size * type_size,
				vertex_buffer->get_size());

			size_t cube_vertex_count = 36; // 36 vertices for a cube (6 faces * 2 triangles per face * 3 vertices per triangle)

			vertex_buffer->resize(cube_vertex_count * max_buffer_size * type_size);
			color_buffer->resize(cube_vertex_count * max_buffer_size * type_size);
			index_buffer->resize(cube_vertex_count * max_buffer_size * sizeof(unsigned int));

			unsigned int *indices = new unsigned int[cube_vertex_count * max_buffer_size];

			int vertex_index = 0;
			int index_offset = 0;

			for (auto voxel : data)
			{
				const float half_size = 0.5f;
				glm::vec3 center = voxel.position;

				// Define the vertices for the cube
				glm::vec3 vertices[] = {
					center + glm::vec3(-half_size, -half_size, -half_size),
					center + glm::vec3(half_size, -half_size, -half_size),
					center + glm::vec3(half_size, half_size, -half_size),
					center + glm::vec3(-half_size, half_size, -half_size),
					center + glm::vec3(-half_size, -half_size, half_size),
					center + glm::vec3(half_size, -half_size, half_size),
					center + glm::vec3(half_size, half_size, half_size),
					center + glm::vec3(-half_size, half_size, half_size)
				};

				// Define the indices for the cube
				unsigned int cube_indices[] = {
					0, 1, 2, 2, 3, 0, // front face
					1, 5, 6, 6, 2, 1, // right face
					5, 4, 7, 7, 6, 5, // back face
					4, 0, 3, 3, 7, 4, // left face
					3, 2, 6, 6, 7, 3, // top face
					4, 5, 1, 1, 0, 4 // bottom face
				};

				for (int i = 0; i < cube_vertex_count; i++)
				{
					glm::vec3 location = vertices[cube_indices[i]];

					vertex_buffer->write(reinterpret_cast<void *>(&location), type_size, vertex_index * type_size);
					color_buffer->write(reinterpret_cast<void *>(&voxel.color), type_size, vertex_index * type_size);

					indices[index_offset + i] = vertex_index;
					vertex_index++;
				}

				index_offset += cube_vertex_count;
			}

			index_buffer->update(reinterpret_cast<void *>(indices));

			delete[] indices;
		}

		void draw()
		{
			if (vertex_buffer->get_size() != 0)
			{
				vertex_buffer->bind_vertex(0, 3);
				color_buffer->bind_vertex(1, 3);
				index_buffer->bind_indices();

				gfx::draw_elements(index_buffer->get_size());
				gfx::draw_arrays(0, vertex_buffer->get_size());
			}
		}
	};

	template<int x, int y, int z>
	class grid
	{
	public:
		grid(glm::vec3 position,
			glm::vec3 size,
			glm::vec3 amount,
			glm::vec3 grid_size)
			: size(size)
			, buffer(glm::vec3(x, y, z))
			, amount(amount)
			, position(position)
			, grid_size(grid_size)
			, max_bound(glm::vec3(x, y, z))
			, min_bound(glm::vec3(0, 0, 0))
		{
			voxels.resize(x * y * z);
		}

		[[nodiscard]] glm::vec3 get_size() const
		{
			return size;
		}

		[[nodiscard]] glm::vec3 get_amount() const
		{
			return amount;
		}

		[[nodiscard]] glm::vec3 get_max_bound() const
		{
			return max_bound;
		}

		[[nodiscard]] glm::vec3 get_min_bound() const
		{
			return min_bound;
		}

		// First = max_bound
		// Second = min_bound
		[[nodiscard]] std::pair<glm::vec3, glm::vec3> get_bounds() const
		{
			return std::make_pair(max_bound, min_bound);
		}

		[[nodiscard]] glm::vec3 get_grid_size() const
		{
			return grid_size;
		}

		glm::vec3 get_chunk_position()
		{
			return position;
		}

		[[nodiscard]] std::optional<voxel_data> get_voxel_at(int lx, int ly, int lz) const
		{
			int index = lx + x * (ly + y * lz);

			if (index < voxels.size() - 2)
			{
				return voxels[index];
			}

			return std::nullopt;
		}

		void set_voxel_at(glm::vec3 position, glm::vec3 color)
		{
			int index = position.x + x * (position.y + y * position.z);
			voxel_data data = {
				.color = color,
				.position = position,
			};

			voxels.at(index) = data;
		}

		void update_buffer(voxel_set data)
		{
			this->buffer.update_buffers(data);
		}

		void draw_buffer()
		{
			this->buffer.draw();
		}

	private:
		const glm::vec3 position;
		const glm::vec3 min_bound;
		const glm::vec3 max_bound;
		const glm::vec3 grid_size;

		const glm::vec3 amount;
		const glm::vec3 size;

		grid_buffer buffer;
		std::vector<std::optional<voxel_data>> voxels;
	};
}
