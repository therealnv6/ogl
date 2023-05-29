#pragma once
#include <functional>
#include <glm/glm.hpp>
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

	private:
		const glm::vec3 origin;
		const glm::vec3 direction;
	};
}
