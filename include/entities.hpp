#pragma once

#include <memory>

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "mesh.hpp"

namespace entity{
	class Geometry{
		public:
			Geometry(glm::vec4 cords, glm::vec4 direction,
				float x_angle, float y_angle, float z_angle,
				float x_scale, float y_scale, float z_scale,
				float x_radius, float z_radius, float height);
			Geometry(glm::vec4 cords);

			virtual ~Geometry();
			//normalize the vectors
			auto translate_foward(float speed) -> void;
			auto translate_direction(glm::vec4 direction, float speed) -> void;
			
			auto rotate_increment(float x_axis, float y_axis, float z_axis) -> void;
			//rotate axis?

			/*
			auto in_2d_bounds(Geometry &geometry) -> bool;
			auto in_3d_bounds(Geometry &geometry) -> bool;
			*/

			inline auto set_cords(float x, float y, float z) -> void {
				cords.x = x; cords.y = y; cords.z = z;
			}
			inline auto set_rotation(float x, float y, float z) -> void {
				x_angle = x; y_angle = y; z_angle = z;
			}

		protected:
			inline auto get_rotaion_ptr() -> float* {
				return glm::value_ptr(rotation);
			}
			inline auto get_translation_ptr() -> float* {
				return glm::value_ptr(translation);
			}

		private:
			//world coordinates
			glm::vec4 cords;
			//vector that points foward 
			glm::vec4 direction;
			//values of the rotation angles for the axis
			float x_angle, y_angle, z_angle;
			//values of the scaling factors
			float x_scale, y_scale, z_scale;
			//values for the bounding box (could be a cilinder, elipsical cilinder or sphere too)
			float x_radius, z_radius ,height;

			glm::mat4 rotation;
			glm::mat4 translation;
			glm::mat4 scaling;

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

	//its inside entity that the draw call happens
	//it passes the parameters to the gpu
	//it has to hold a reference to:
	//	- the gpu program
	//	- the Mesh it renders
	class Entity : public Geometry{
		public:
			//usar uma lista de atributos? ou fazer default
			Entity();

			auto draw()->bool;
		private:
			std::shared_ptr<render::ObjMesh> mesh;
			std::shared_ptr<render::WireMesh> mesh;
	};

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