#include "voxel/ray.hpp"
#include <voxel/tracing.hpp>

namespace ray
{
	std::vector<voxel_data> trace_rays(const ray::raycast &ray, const voxel::grid &grid, glm::vec2 bounds)
	{
		std::vector<voxel_data> data;
		spdlog::info("trace rays");
		// todo: implement this

		return data;
	}
}
