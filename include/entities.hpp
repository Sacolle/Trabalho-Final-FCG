#pragma once
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace entity{
	class Camera {
		public:
			Camera(bool is_perspective);
			auto update_position(float phi, float theta, float radius) -> void;
			inline auto get_projection_ptr() -> float* { return glm::value_ptr(projection); }
			inline auto get_view_ptr() -> float* { return glm::value_ptr(view); }
		private:
			glm::vec4 point_c;
			glm::vec4 up_vec;
			glm::vec4 point_look_at;

			glm::mat4 projection;
			glm::mat4 view;
	};
}