#pragma once

#include <voxel/grid.hpp>
#include <voxel/ray.hpp>

namespace ray
{
	template<int X, int Y, int Z>
	std::optional<voxel::voxel_data> trace_ray(
		const ray::raycast &ray,
		const voxel::grid<X, Y, Z> &grid,
		glm::vec3 deltas,
		glm::vec3 steps,
		glm::vec3 chunk_local_position);

	std::optional<voxel::voxel_data> trace_ray(
		const ray::raycast &ray,
		const voxel::grid<16, 16, 16> &grid,
		glm::vec3 deltas,
		glm::vec3 steps,
		glm::vec3 chunk_local_position);
}
