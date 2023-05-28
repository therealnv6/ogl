#pragma once

#include <voxel/grid.hpp>
#include <voxel/ray.hpp>

namespace ray
{
	struct voxel_data {
		glm::vec3 color;
		glm::vec3 position;
	};

	std::vector<voxel_data> trace_rays(
		const ray::raycast &ray,
		const voxel::grid &grid,
		glm::vec2 bounds // bounds.second has to be >= 1.0 and bounds.first has to be < bounds.second
	);
}
