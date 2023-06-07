#include "input.hpp"
#include "voxel/ray.hpp"
#include <voxel/tracing.hpp>

namespace ray
{
	static const int VIEW_DISTANCE = 50;
	static const glm::vec3 VIEW_DISTANCE_VEC = glm::vec3(static_cast<float>(VIEW_DISTANCE));

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

		auto [deltaX, deltaY, deltaZ] = std::make_tuple(0.0f, 0.0f, 0.0f);

		std::optional<voxel::voxel_data> data = std::nullopt;

		// Calculate the maximum distance along the ray
		float tMaxX = (direction.x > 0) ? (max_bound.x - start_position.x) / direction.x : (min_bound.x - start_position.x) / direction.x;
		float tMaxY = (direction.y > 0) ? (max_bound.y - start_position.y) / direction.y : (min_bound.y - start_position.y) / direction.y;
		float tMaxZ = (direction.z > 0) ? (max_bound.z - start_position.z) / direction.z : (min_bound.z - start_position.z) / direction.z;

		// Calculate the distance to move along the ray for each step
		float tDeltaX = std::abs(deltas.x / direction.x);
		float tDeltaY = std::abs(deltas.y / direction.y);
		float tDeltaZ = std::abs(deltas.z / direction.z);

		// Determine the direction to step along the ray
		int stepX = (direction.x > 0) ? 1 : -1;
		int stepY = (direction.y > 0) ? 1 : -1;
		int stepZ = (direction.z > 0) ? 1 : -1;

		// Perform ray marching
		glm::vec3 current_position = start_position;

		while (!data.has_value() && glm::all(glm::lessThan(glm::abs(current_position - start_position), VIEW_DISTANCE_VEC)))
		{
			int x = static_cast<int>((current_position.x - min_bound.x) / deltas.x + 0.5f);
			int y = static_cast<int>((current_position.y - min_bound.y) / deltas.y + 0.5f);
			int z = static_cast<int>((current_position.z - min_bound.z) / deltas.z + 0.5f);

			data = grid.get_voxel_at(x, y, z);

			if (tMaxX < tMaxY)
			{
				if (tMaxX < tMaxZ)
				{
					current_position.x += stepX * deltas.x;
					tMaxX += tDeltaX;
				}
				else
				{
					current_position.z += stepZ * deltas.z;
					tMaxZ += tDeltaZ;
				}
			}
			else
			{
				if (tMaxY < tMaxZ)
				{
					current_position.y += stepY * deltas.y;
					tMaxY += tDeltaY;
				}
				else
				{
					current_position.z += stepZ * deltas.z;
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

	std::optional<voxel::voxel_data> trace_ray(
		const ray::raycast &ray,
		const voxel::grid<32, 32, 32> &grid,
		glm::vec3 deltas,
		glm::vec3 steps,
		glm::vec3 start_position)
	{
		return trace_ray<32, 32, 32>(ray, grid, deltas, steps, start_position);
	}
}
