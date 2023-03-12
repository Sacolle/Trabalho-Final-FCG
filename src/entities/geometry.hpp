#pragma once

#include <memory>
#include <utility>

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../utils/matrix.hpp"

namespace entity{
	enum class BBoxType{
		Rectangle,
		Cylinder,
		Sphere
	};
	class Geometry{
		public:
			Geometry(glm::vec4 cords, glm::vec4 direction, float speed,
				float x_angle, float y_angle, float z_angle,
				float x_scale, float y_scale, float z_scale,
				float x_radius, float z_radius, float height, BBoxType bbox_type);
			Geometry(glm::vec4 cords);
			Geometry();

			virtual ~Geometry();
			//normalize the vectors
			auto translate_foward(const float delta_time) -> void;
			auto translate_direction(const glm::vec4 dir, const float delta_time) -> void;

			auto rotate_increment(float x, float y, float z) -> void;
			auto set_cords(float x, float y, float z) -> void;
			auto set_angles(float x, float y, float z) -> void;
			auto set_y_angle(float y) -> void;
			auto set_scale(float x, float y, float z) -> void;
			auto set_direction(glm::vec4 new_dir) -> void;

			inline auto set_speed(float s) -> void { speed = s; }
			inline auto get_speed() const -> float { return speed; }
			
			inline auto set_base_translate(float x, float y, float z) -> void {
				base_translate = mtx::translate(x,y,z);
			}
			inline auto get_cords() const -> glm::vec4 { return cords; }

			inline auto get_direction() const -> glm::vec4 { return direction; }
			inline auto get_parcial_direction_x() const -> glm::vec4 { return glm::vec4(direction.x,0,0,0); }
			inline auto get_parcial_direction_y() const -> glm::vec4 { return glm::vec4(0,direction.y,0,0); }
			inline auto get_parcial_direction_z() const -> glm::vec4 { return glm::vec4(0,0,direction.z,0); }
			
			inline auto set_base_direction(glm::vec4 dir) -> void { base_direction = dir; }
			inline auto get_base_direction() const -> glm::vec4 { return base_direction; }


			//bbox elements
			inline auto get_bbox_type() const -> BBoxType { return bbox_type; }
			inline auto set_bbox_type(BBoxType bbox) -> void { bbox_type = bbox; }

			inline auto get_x_radius() const -> float { return x_radius; }
			inline auto get_height() const -> float { return height; }
			inline auto get_z_radius() const -> float { return z_radius; }

			inline auto set_x_radius(float x) -> void { x_radius = x; }
			inline auto set_height(float y) -> void { height = y; }
			inline auto set_z_radius(float z) -> void { z_radius = z; }

			inline auto set_bbox_size(float x, float y, float z) -> void {x_radius = x; height = y; z_radius = z; }
			inline auto get_transform() const -> glm::mat4 {
				return translation * rotation * scaling * base_translate;
			}
			inline auto get_translation() const -> glm::mat4 {
				return translation;
			} 
			inline auto get_bbox_scale() const -> glm::mat4 {
				return mtx::scale(x_radius,height,z_radius);
			}
		private:
			//world coordinates
			glm::vec4 cords;
			//vector that points foward 
			glm::vec4 direction;
			glm::vec4 base_direction;
			//values of the rotation angles for the axis
			float x_angle, y_angle, z_angle;
			//values of the scaling factors
			float x_scale, y_scale, z_scale;
			//values for the bounding box (could be a cilinder, elipsical cilinder or sphere too)
			float x_radius, z_radius ,height;
			BBoxType bbox_type;
			//how much it translates in a step
			float speed;

			glm::mat4 rotation;
			glm::mat4 translation;
			glm::mat4 scaling;
			glm::mat4 base_translate; //correcting translation for when models origins are not at their center

			inline auto set_rotation() -> void{
				rotation = mtx::rot_z(z_angle) * mtx::rot_y(y_angle) * mtx::rot_x(x_angle);
			}
			inline auto set_translation() -> void{
				translation = mtx::translate(cords.x,cords.y,cords.z);
			}
			inline auto set_scaling() -> void{
				scaling = mtx::scale(x_scale, y_scale, z_scale);
			}
	};
}