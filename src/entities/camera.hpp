#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../utils/matrix.hpp"

namespace entity{
	class Camera {
		public:
			Camera();
			auto update_aspect_ratio(float aspect_ratio) -> void;
			auto update_position(float phi, float theta, float radius) -> void;
			auto update_position() -> void;
			inline auto set_angles(float _phi, float _theta, float _radius) -> void {
				phi = _phi;
				theta = _theta;
				radius = _radius;
			}
			inline auto get_projection_ptr() -> float* { return glm::value_ptr(projection); }
			inline auto get_view_ptr() -> float* { return glm::value_ptr(view); }
		private:
			bool free_cam;

			glm::vec4 up_vec;
			float theta;
			float phi;
			float radius;
			//glm::vec4 direction; //used as point_c and view_vector
			
			//for free cam
			glm::vec4 camera_position;

			//for look_at camera
			glm::vec4 point_look_at; 

			float aspect_ratio;
			float near_plane;
			float far_plane;

			glm::mat4 projection;
			glm::mat4 view;
	};
}