#pragma once
#include <buffer.hpp>
#include <glm/glm.hpp>
#include <render.hpp>
#include <voxel/ray.hpp>

namespace svo
{
	struct voxel {
		glm::vec3 position;
		glm::vec3 color;
		float size;
	};

	struct node {
		voxel voxels[8];
		node *children[8] = { nullptr };
	};

	class svo
	{
	public:
		/**
		 * Constructs an SVO with a root voxel.
		 *
		 * @param position   The position of the root voxel.
		 * @param color      The color of the root voxel.
		 * @param root_size  The size of the root voxel.
		 *
		 * @remarks This constructor initializes the SVO with a root voxel at the specified position, color, and size.
		 */
		svo(const glm::vec3 &position, const glm::vec3 &color, float root_size)
			: buffer(buffer::buffer(nullptr, 0, draw_type::dynamic_draw, buffer_type::shader_storage))
		{
			root = new node();
			root->voxels[0] = voxel { position, color, root_size };
		}

		/**
		 * Subdivides a node into eight children nodes.
		 *
		 * @param node  The node to subdivide.
		 *
		 * @remarks This function subdivides the specified
		 *          node into eight children nodes.
		 */
		void subdivide_node(node *node)
		{
			const glm::vec3 &parent_pos = node->voxels[0].position;

			float parent_size = node->voxels[0].size;
			float child_size = parent_size / 2;

			glm::vec3 child_color = node->voxels[0].color;

			for (int i = 0; i < 8; i++)
			{
				glm::vec3 child_position = parent_pos;
				child_position.x += (i & 1) ? child_size : -child_size;
				child_position.y += (i & 2) ? child_size : -child_size;
				child_position.z += (i & 4) ? child_size : -child_size;

				node->children[i] = new class node();
				node->children[i]->voxels[0] = voxel { child_position, child_color, child_size };
			}
		}

		/**
		 * Constructs the octree recursively by subdividing nodes.
		 *
		 * @remarks This function constructs the octree recursively by subdividing nodes
		 *          until the minimum voxel size is reached.
		 */
		void construct_octree()
		{
			construct_octree_recursive(root);
		}

		/**
		 * Constructs the octree recursively by subdividing nodes.
		 *
		 * @param node  The current node being processed.
		 *
		 * @remarks This function constructs the octree recursively
		 *          by subdividing nodes until the minimum voxel size
		 *          for the sparse voxel octree is reached.
		 */
		void construct_octree_recursive(node *node)
		{
			if (node->voxels[0].size <= min_voxel_size)
			{
				return;
			}

			subdivide_node(node);

			for (int i = 0; i < 8; i++)
			{
				if (node->children[i])
				{
					construct_octree_recursive(node->children[i]);
				}
			}
		}

		float march(ray::raycast &ray, float max_distance) const
		{
			return march_recursive(ray, max_distance, root);
		}

		/**
		 * Performs ray marching recursively through the octree to find the distance to the nearest surface.
		 *
		 * @param ray           The ray to march.
		 * @param max_distance  The maximum distance to march.
		 * @param node          The current node being processed.
		 * @return The distance to the nearest surface.
		 *
		 * @remarks This function performs ray marching recursively
		 *          through the octree to find the distance to the nearest surface.
		 */
		float march_recursive(ray::raycast &ray, float max_distance, const node *node) const
		{
			float min = std::numeric_limits<float>::max();

			if (!node->children[0])
			{
				for (int i = 0; i < 8; i++)
				{
					const auto &voxel = node->voxels[i];
					if (voxel.size > 0.0f)
					{
						float t = ray.intersect_cube(voxel.position, voxel.size);
						if (t >= 0.0f && t < min)
						{
							min = t;
						}
					}
				}

				return min;
			}
			else
			{
				for (int i = 0; i < 8; i++)
				{
					const auto &child = node->children[i];
					if (child)
					{
						float t = ray.intersect_cube(child->voxels[0].position, child->voxels[0].size);
						if (t >= 0.0f && t < min)
						{
							min = t;
							ray.set_origin(ray.get_origin() + ray.get_direction() * t);

							auto result = march_recursive(ray, max_distance - t, child);

							if (result >= 0.0f)
							{
								return t + result;
							}
						}
					}
				}
				return -1.0f;
			}
		}

		void bind_to_gpu(buffer::buffer *buffer)
		{
		}

	private:
		node *root;
		float min_voxel_size = 0.01f;
		buffer::buffer buffer;
	};
};
