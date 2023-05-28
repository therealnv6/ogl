#pragma once
#include <functional>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>
#include <vector>

namespace voxel
{
	class grid
	{
		grid(glm::vec3 size, glm::vec3 amount, glm::vec3 grid_size, glm::vec3 max_bound, glm::vec3 min_bound)
			: size(size)
			, amount(amount)
			, grid_size(grid_size)
			, max_bound(max_bound)
			, min_bound(min_bound)
		{
			spdlog::debug("created new grid");
		}

		[[nodiscard]] glm::vec3 get_size()
		{
			return size;
		}

		[[nodiscard]] glm::vec3 get_amount()
		{
			return amount;
		}

		[[nodiscard]] glm::vec3 get_max_bound()
		{
			return max_bound;
		}

		[[nodiscard]] glm::vec3 get_min_bound()
		{
			return min_bound;
		}

		// First = max_bound
		// Second = min_bound
		[[nodiscard]] std::pair<glm::vec3, glm::vec3> get_bounds()
		{
			return std::make_pair(max_bound, min_bound);
		}

		[[nodiscard]] glm::vec3 get_grid_size()
		{
			return grid_size;
		}

	private:
		const glm::vec3 min_bound;
		const glm::vec3 max_bound;
		const glm::vec3 grid_size;

		const glm::vec3 amount;
		const glm::vec3 size;
	};
}
