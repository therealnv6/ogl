#pragma once
#include <glm/glm.hpp>
#include <voxel/ray.hpp>

namespace svo
{
	struct voxel {
		glm::vec3 position;
		glm::vec3 color;
		float size;

		voxel(const glm::vec3 &pos, const glm::vec3 &color, float size)
			: position(pos)
			, color(color)
			, size(size)
		{
		}
	};

	class node
	{
	public:
		node(bool leaf = true)
		{
			this->leaf = leaf;
		}

		bool leaf;
		std::vector<voxel> voxels;
		node *children[8];
	};

	class svo
	{
	public:
		svo(const glm::vec3 &rootPosition, const glm::vec3 &color, float rootSize)
		{
			root = new node();
			root->leaf = true;
			root->voxels.emplace_back(rootPosition, color, rootSize);
		}

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
				node->children[i]->leaf = true;
				node->children[i]->voxels.emplace_back(child_position, child_color, child_size);
			}

			node->leaf = false;
		}

		void construct_octree()
		{
			construct_octree_recursive(root);
		}

		void construct_octree_recursive(node *node)
		{
			if (node->voxels[0].size <= min_voxel_size)
			{
				return;
			}

			subdivide_node(node);

			for (int i = 0; i < 8; i++)
			{
				construct_octree_recursive(node->children[i]);
			}
		}

		float march(ray::raycast &ray, float max_distance) const
		{
			return march_recursive(ray, max_distance, root);
		}

		float march_recursive(ray::raycast &ray, float max_distance, const node *node) const
		{
			float min = std::numeric_limits<float>::max();

			if (node->leaf)
			{

				for (const auto &voxel : node->voxels)
				{
					float t = ray.intersect_cube(voxel.position, voxel.size);
					if (t >= 0.0f && t < min)
					{
						min = t;
					}
				}
				return min;
			}
			else
			{
				for (int i = 0; i < 8; i++)
				{
					float t = ray.intersect_cube(node->children[i]->voxels[0].position, node->children[i]->voxels[0].size);
					if (t >= 0.0f && t < min)
					{
						min = t;
						ray.set_origin(ray.get_origin() + ray.get_direction() * t);

						auto result = march_recursive(ray, max_distance - t, node->children[i]);

						if (result >= 0.0f)
						{
							return t + result;
						}
					}
				}
				return -1.0f;
			}
		}

	private:
		node *root;
		float min_voxel_size = 0.01f;
	};
};
