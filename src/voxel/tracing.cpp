#include "voxel/ray.hpp"
#include <voxel/tracing.hpp>

namespace ray
{
	template<int X, int Y, int Z>
	voxel::voxel_data *trace_ray(const ray::raycast &ray,
		const voxel::grid<X, Y, Z> &grid,
		glm::vec3 deltas,
		glm::vec3 steps)
	{
		glm::vec3 origin = ray.get_origin();
		glm::vec3 max_bound = grid.get_max_bound();

		auto [x, y, z] = std::make_tuple(origin.x, origin.y, origin.z);
		auto [deltaX, deltaY, deltaZ] = std::make_tuple(0.0f, 0.0f, 0.0f);

		voxel::voxel_data *data;

		do
		{
			if (deltaX < deltaY)
			{
				if (deltaX < deltaZ)
				{
					x += steps.x;
					deltaX += deltas.x;
				}
				else
				{
					z += steps.z;
					deltaZ += deltas.z;
				}
			}
			else
			{
				if (deltaY < deltaZ)
				{
					y += steps.y;
					deltaY += deltas.z;
				}
				else
				{
					z += steps.z;
					deltaZ += deltas.z;
				}
			}

			if (x >= max_bound.x || y >= max_bound.y || z >= max_bound.z)
			{
				break;
			}

			data = grid.get_voxel_at(x, y, z);
		} while (data != nullptr);

		return data;
	}
}
