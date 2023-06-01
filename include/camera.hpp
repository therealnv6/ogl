#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace gfx
{
	class camera
	{
	public:
		[[nodiscard]] glm::mat4 get_view_matrix() const
		{
			return glm::lookAt(position, position + direction, up);
		}

		[[nodiscard]] glm::mat4 get_projection(float aspect_ratio) const
		{
			return glm::perspective(glm::radians(45.0f), aspect_ratio, 0.1f, 100000.0f);
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

	private:
		glm::vec3 position = glm::vec3(0.0f, 0.0f, 5.0f);
		glm::vec3 direction = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	};
}
