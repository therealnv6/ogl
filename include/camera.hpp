#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <stdexcept>

enum class projection
{
	perspective,
	orthographic
};

namespace gfx
{

	class camera
	{
	public:
		camera(projection projection_type = projection::perspective,
			float fov = 45.0f,
			float near_plane = 0.1f,
			float far_plane = 100000.0f)
			: projection_type(projection_type)
			, fov(fov)
			, near_plane(near_plane)
			, far_plane(far_plane)
		{
		}

		camera(projection projection_type,
			float left, float right, float bottom, float top,
			float near_plane = 0.1f, float far_plane = 100000.0f)
			: projection_type(projection_type)
			, left(left)
			, right(right)
			, bottom(bottom)
			, top(top)
			, near_plane(near_plane)
			, far_plane(far_plane)
		{
		}

		camera(projection projection_type)
			: projection_type(projection_type)
		{
		}

		[[nodiscard]] glm::mat4 get_view_matrix() const
		{
			return glm::lookAt(position, position + direction, up);
		}

		[[nodiscard]] glm::mat4 get_projection(float aspect_ratio = 16.0 / 9.0) const
		{
			if (projection_type == projection::perspective)
			{
				return glm::perspective(glm::radians(fov), aspect_ratio, near_plane, far_plane);
			}
			else if (projection_type == projection::orthographic)
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

		void rotate_to(float yaw, float pitch)
		{
			// Convert yaw and pitch from degrees to radians
			yaw = glm::radians(yaw);
			pitch = glm::radians(pitch);

			// Calculate the new direction vector
			glm::vec3 newDirection;
			newDirection.x = cos(yaw) * cos(pitch);
			newDirection.y = sin(pitch);
			newDirection.z = sin(yaw) * cos(pitch);
			direction = glm::normalize(newDirection);

			// Recalculate the up vector
			up = glm::normalize(glm::cross(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), direction), direction));
		}

		glm::vec3 get_position()
		{
			return position;
		}

	private:
		projection projection_type;

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
