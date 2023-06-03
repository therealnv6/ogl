#pragma once
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <iostream>
#include <spdlog/spdlog.h>
#include <vector>

namespace voxel
{
	struct voxel_data {
		glm::vec3 color;
		glm::vec3 position;
	};

	template<int x, int y, int z>
	class grid
	{
	public:
		grid(glm::vec3 size, glm::vec3 amount, glm::vec3 grid_size)
			: size(size)
			, amount(amount)
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

		[[nodiscard]] std::optional<voxel_data> get_voxel_at(int lx, int ly, int lz) const
		{
			int index = lx + x * (ly + y * lz);
			auto data = voxels[index];

			return data;
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

	private:
		const glm::vec3 min_bound;
		const glm::vec3 max_bound;
		const glm::vec3 grid_size;

		const glm::vec3 amount;
		const glm::vec3 size;

		std::vector<std::optional<voxel_data>> voxels;
	};
}
