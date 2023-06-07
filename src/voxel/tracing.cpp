#include "input.hpp"
#include "voxel/ray.hpp"
#include <voxel/tracing.hpp>

namespace ray
{
	template<int X, int Y, int Z>
	std::optional<voxel::voxel_data> trace_ray(
		const ray::raycast &ray,
		const voxel::grid<X, Y, Z> &grid,
		glm::vec3 deltas,
		glm::vec3 steps,
		glm::vec3 start_position)
	{
		glm::vec3 origin = ray.get_origin();
		glm::vec3 direction = ray.get_direction();
		glm::vec3 max_bound = grid.get_max_bound();
		glm::vec3 min_bound = grid.get_min_bound();

		// Calculate the initial position on the ray
		glm::vec3 position = start_position;

		auto [x, y, z] = std::make_tuple(position.x, position.y, position.z);
		auto [deltaX, deltaY, deltaZ] = std::make_tuple(0.0f, 0.0f, 0.0f);

		std::optional<voxel::voxel_data> data = std::nullopt;

		// Calculate the maximum distance along the ray
		float tMaxX = (direction.x > 0) ? (max_bound.x - origin.x) / direction.x : (min_bound.x - origin.x) / direction.x;
		float tMaxY = (direction.y > 0) ? (max_bound.y - origin.y) / direction.y : (min_bound.y - origin.y) / direction.y;
		float tMaxZ = (direction.z > 0) ? (max_bound.z - origin.z) / direction.z : (min_bound.z - origin.z) / direction.z;

		// Calculate the distance to move along the ray for each step
		float tDeltaX = std::abs(deltas.x / direction.x);
		float tDeltaY = std::abs(deltas.y / direction.y);
		float tDeltaZ = std::abs(deltas.z / direction.z);

		// Determine the direction to step along the ray
		int stepX = (direction.x > 0) ? 1 : -1;
		int stepY = (direction.y > 0) ? 1 : -1;
		int stepZ = (direction.z > 0) ? 1 : -1;

		// Perform ray marching
		while (x >= min_bound.x && y >= min_bound.y && z >= min_bound.z && x < max_bound.x && y < max_bound.y && z < max_bound.z)
		{
			data = grid.get_voxel_at(static_cast<int>(x), static_cast<int>(y), static_cast<int>(z));
			if (data.has_value())
			{
				break;
			}

			if (tMaxX < tMaxY)
			{
				if (tMaxX < tMaxZ)
				{
					x += stepX;
					tMaxX += tDeltaX;
				}
				else
				{
					z += stepZ;
					tMaxZ += tDeltaZ;
				}
			}
			else
			{
				if (tMaxY < tMaxZ)
				{
					y += stepY;
					tMaxY += tDeltaY;
				}
				else
				{
					z += stepZ;
					tMaxZ += tDeltaZ;
				}
			}
		}

		return data;
	}

	std::optional<voxel::voxel_data> trace_ray(
		const ray::raycast &ray,
		const voxel::grid<16, 16, 16> &grid,
		glm::vec3 deltas,
		glm::vec3 steps,
		glm::vec3 start_position)
	{
		return trace_ray<16, 16, 16>(ray, grid, deltas, steps, start_position);
	}
}
