#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../utils/matrix.hpp"
#include "../entities/entity.hpp"

#define TOTAL_DIRECTIONS 4

namespace entity{
	typedef struct LookAtParameters{
		float phi, theta, radius;
	} LookAtParameters;
	typedef struct RotationAngles{
		float angleX, angleY;
	} RotationAngles;
	class Camera {
		public:
			Camera();
			auto update_aspect_ratio(float aspect_ratio) -> void;
			auto update_position(LookAtParameters &parameters) -> void;
			auto update_position(entity::PressedKeys &keys, float delta_time) -> void;
			auto update_direction(RotationAngles &angles, float delta_time) -> void;
			inline auto set_angles(LookAtParameters &parameters) -> void {
				look_at_parameters.phi = parameters.phi;
				look_at_parameters.theta = parameters.theta;
				look_at_parameters.radius = parameters.radius;
			}
			inline auto get_projection_ptr() -> float* { return glm::value_ptr(projection); }
			inline auto get_view_ptr() -> float* { return glm::value_ptr(view); }
		private:
			glm::vec4 up_vec;
			LookAtParameters look_at_parameters;
			glm::vec4 camera_direction; //used as point_c and view_vector
			
			//for free cam
			glm::vec4 camera_position;
			float cam_move_speed;
			float cam_look_speed;

			//for look_at camera
			glm::vec4 point_look_at; 

			float aspect_ratio;
			float near_plane;
			float far_plane;


			glm::mat4 projection;
			glm::mat4 view;

			auto sum_direction(entity::PressedKeys &keys) -> float;
	};
}