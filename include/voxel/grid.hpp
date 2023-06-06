#pragma once
#include "buffer.hpp"
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

	public:
		grid_buffer(glm::vec3 bounds)
		{
			vertex_buffer = new buffer::buffer(nullptr, 0, draw_type::Dynamic);
			color_buffer = new buffer::buffer(nullptr, 0, draw_type::Dynamic);
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

			vertex_buffer->resize(max_buffer_size * type_size);
			color_buffer->resize(max_buffer_size * type_size);

			int i = 0;

			for (auto voxel : data)
			{
				vertex_buffer->write(reinterpret_cast<void *>(&voxel.position), type_size, i * type_size);
				color_buffer->write(reinterpret_cast<void *>(&voxel.color), type_size, i * type_size);
				i++;
			}
		}

		void draw()
		{
			if (vertex_buffer->get_size() != 0)
			{
				vertex_buffer->bind_vertex(0, 3);
				color_buffer->bind_vertex(1, 3);

				gfx::draw_arrays(0, vertex_buffer->get_size() + 128);
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
