#pragma once
#include <functional>
#include <glm/glm.hpp>
#include <vector>

static int NUMBER_OF_STEPS = 32;
static float MINIMUM_HIT_DIST = 0.001;
static float MAXIMUM_TRACE_DIST = 1000.0;

namespace voxel
{
	class ray
	{
	private:
		float box_sdf(glm::vec3 p, glm::vec3 b)
		{
			glm::vec3 q = glm::abs(p) - b;
			return glm::length(glm::max(q, glm::vec3(0.0f))) + glm::min(glm::max(q.x, glm::max(q.y, q.z)), 0.0f);
		}

		glm::vec3 calculate_normal(glm::vec3 point, glm::vec3 position)
		{
			const glm::vec3 small_step = glm::vec3(0.001, 0.0, 0.0);

			float gradient_x = box_sdf(point + glm::vec3(small_step.x, small_step.y, small_step.y), position) - box_sdf(point - glm::vec3(small_step.x, small_step.y, small_step.y), position);
			float gradient_y = box_sdf(point + glm::vec3(small_step.y, small_step.x, small_step.y), position) - box_sdf(point - glm::vec3(small_step.y, small_step.x, small_step.y), position);
			float gradient_z = box_sdf(point + glm::vec3(small_step.y, small_step.y, small_step.x), position) - box_sdf(point - glm::vec3(small_step.y, small_step.y, small_step.x), position);

			return glm::normalize(glm::vec3(gradient_x, gradient_y, gradient_z));
		}

	public:
		// we'll move this to the gpu eventually
		glm::vec3 march(glm::vec3 first, glm::vec3 second, glm::vec3 position)
		{
			float distance_traveled = 0.0;

			for (int i = 0; i < NUMBER_OF_STEPS; i++)
			{
				glm::vec3 current = first + distance_traveled * second;
				float closest = box_sdf(current, position);

				if (closest < MINIMUM_HIT_DIST)
				{
					return calculate_normal(current, position);
				}

				if (distance_traveled > MAXIMUM_TRACE_DIST)
				{
					break;
				}

				distance_traveled += closest;
			}

			return glm::vec3(0.0);
		}
	};

	class world
	{
	private:
		glm::vec3 voxel_size;
		glm::vec3 world_size;
		glm::ivec3 voxel_count;
		std::vector<bool> voxels;

	public:
		world(glm::vec3 voxelSize, glm::vec3 worldSize)
			: voxel_size(voxelSize)
			, world_size(worldSize)
		{
			voxel_count = glm::ceil(worldSize / voxelSize);
			voxels.resize(voxel_count.x * voxel_count.y * voxel_count.z, false);
		}

		void set_at(int x, int y, int z, bool value)
		{
			if (x >= 0 && x < voxel_count.x && y >= 0 && y < voxel_count.y && z >= 0 && z < voxel_count.z)
			{
				voxels[x + voxel_count.x * (y + voxel_count.y * z)] = value;
			}
		}

		bool get_at(int x, int y, int z) const
		{
			if (x >= 0 && x < voxel_count.x && y >= 0 && y < voxel_count.y && z >= 0 && z < voxel_count.z)
			{
				return voxels[x + voxel_count.x * (y + voxel_count.y * z)];
			}
			return false;
		}

		void render(
			const std::function<void(const glm::vec3 &, const glm::vec3 &)> &render,
			const glm::vec3 &position,
			const glm::vec3 &direction)
		{
			glm::vec3 ray_origin = position;
			glm::vec3 ray_direction = glm::normalize(direction);

			ray marching_ray;

			for (int z = 0; z < voxel_count.z; z++)
			{
				for (int y = 0; y < voxel_count.y; y++)
				{
					for (int x = 0; x < voxel_count.x; x++)
					{
						glm::vec3 voxel_position = glm::vec3(x, y, z) * voxel_size;
						bool voxel_value = get_at(x, y, z);

						if (voxel_value)
						{
							glm::vec3 normal = marching_ray.march(ray_origin, ray_direction, voxel_position);
							render(voxel_position, normal);
						}
					}
				}
			}
		}
	};
}
