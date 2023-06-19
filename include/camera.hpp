#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <stdexcept>

namespace gfx
{
	enum class projection
	{
		perspective,
		orthographic
	};

	class camera
	{
	public:
		camera(projection projection_type = projection::perspective,
			float fov = 45.0f,
			float near_plane = 0.1f,
			float far_plane = 100000.0f)
			: projection(projection_type)
			, fov(fov)
			, near_plane(near_plane)
			, far_plane(far_plane)
		{
		}

		camera(projection projection_type,
			float left, float right, float bottom, float top,
			float near_plane = 0.1f, float far_plane = 100000.0f)
			: projection(projection_type)
			, left(left)
			, right(right)
			, bottom(bottom)
			, top(top)
			, near_plane(near_plane)
			, far_plane(far_plane)
		{
		}

		camera(projection projection_type)
			: projection(projection_type)
		{
		}

		[[nodiscard]] glm::mat4 get_view_matrix() const
		{
			return glm::lookAt(position, position + direction, up);
		}

		[[nodiscard]] glm::mat4 get_projection(float aspect_ratio = 16.0 / 9.0) const
		{
			if (projection == projection::perspective)
			{
				return glm::perspective(glm::radians(fov), aspect_ratio, near_plane, far_plane);
			}
			else if (projection == projection::orthographic)
			{
				return glm::ortho(left, right, bottom, top, near_plane, far_plane);
			}

			throw std::runtime_error("unable to find projection");
		}

		[[nodiscard]] glm::vec3 get_direction() const
		{
			return direction;
		}

		[[nodiscard]] glm::vec3 forward()
		{
			return glm::cross(direction, up);
		}

		[[nodiscard]] glm::vec3 get_up() const
		{
			return up;
		}

		void move(glm::vec3 position)
		{
			this->position = position;
		}

		void rotate(float yaw, float pitch)
		{
			glm::quat yaw_quat = glm::angleAxis(yaw, up);
			glm::quat pitch_quat = glm::angleAxis(pitch, glm::cross(up, direction));

			// Update the direction vector
			glm::quat rotation_quat = yaw_quat * pitch_quat;
			direction = rotation_quat * direction;

			// Update the up vector
			up = rotation_quat * up;
		}

		glm::vec3 get_position()
		{
			return position;
		}

	private:
		gfx::projection projection;
		glm::vec3 position = glm::vec3(0.0f, 0.0f, 5.0f);
		glm::vec3 direction = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

		// Perspective projection parameters
		float fov = 45.0f;
		float near_plane = 0.1f;
		float far_plane = 100000.0f;

		// Orthographic projection parameters
		float left = -10.0f;
		float right = 10.0f;
		float bottom = -10.0f;
		float top = 10.0f;
	};
}
