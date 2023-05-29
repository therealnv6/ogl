#pragma once

#include <voxel/grid.hpp>
#include <voxel/ray.hpp>

namespace ray
{
	template<int X, int Y, int Z>
	voxel::voxel_data *trace_ray(
		const ray::raycast &ray,
		const voxel::grid<X, Y, Z> &grid,
		glm::vec3 deltas,
		glm::vec3 steps);
}
