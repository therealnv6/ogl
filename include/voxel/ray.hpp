#pragma once
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <spdlog/spdlog.h>
#include <vector>

namespace ray
{
	struct raycast final {
	public:
		[[nodiscard]] constexpr raycast(glm::vec3 origin, glm::vec3 direction)
			: origin(origin)
			, direction(direction)
		{
			spdlog::debug("created new ray");
		};

		[[nodiscard]] glm::vec3 point_at(float param) const
		{
			return origin + (direction * param);
		}

		[[nodiscard]] glm::vec3 get_origin() const
		{
			return origin;
		}

		[[nodiscard]] glm::vec3 get_direction() const
		{
			return direction;
		}

		void set_origin(glm::vec3 origin)
		{
			this->origin = origin;
		}

		void set_direction(glm::vec3 direction)
		{
			this->direction = direction;
		}

		void rotate_yaw(float angle_degrees)
		{
			float angle_rad = glm::radians(angle_degrees);

			glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angle_rad, glm::vec3(0.0f, 1.0f, 0.0f));

			direction = glm::vec3(rotation * glm::vec4(direction, 0.0f));
			direction = glm::normalize(direction);
		}

		void rotate_pitch(float angle_degrees)
		{
			float angle_rad = glm::radians(angle_degrees);

			glm::vec3 right = glm::cross(direction, glm::vec3(0.0f, 1.0f, 0.0f));
			glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angle_rad, right);

			direction = glm::vec3(rotation * glm::vec4(direction, 0.0f));
			direction = glm::normalize(direction);
		}

	private:
		glm::vec3 origin;
		glm::vec3 direction;
	};
}
