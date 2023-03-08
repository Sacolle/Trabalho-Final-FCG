#pragma once

#include <memory>
#include <utility>

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "mesh.hpp"
#include "matrix.hpp"
#include "shader.hpp"

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

			virtual ~Geometry();
			//normalize the vectors
			auto translate_foward() -> void;
			auto translate_direction(const glm::vec4 dir) -> void;

			auto rotate_increment(float x, float y, float z) -> void;
			auto set_cords(float x, float y, float z) -> void;
			auto set_angles(float x, float y, float z) -> void;
			auto set_y_angle(float y) -> void;


			inline auto set_speed(float s) -> void { speed = s; }
			inline auto get_speed() const -> float { return speed; }
			
			inline auto set_base_translate(float x, float y, float z) -> void {
				base_translate = mtx::translate(x,y,z);
			}
			inline auto get_cords() const -> glm::vec4 { return cords; }
			inline auto get_x_cords() const -> float { return cords.x; }
			inline auto get_y_cords() const -> float { return cords.y; }
			inline auto get_z_cords() const -> float { return cords.z; }

			inline auto get_direction() const -> glm::vec4 { return direction; }
			inline auto get_parcial_direction_x() const -> glm::vec4 { return glm::vec4(direction.x,0,0,0); }
			inline auto get_parcial_direction_y() const -> glm::vec4 { return glm::vec4(0,direction.y,0,0); }
			inline auto get_parcial_direction_z() const -> glm::vec4 { return glm::vec4(0,0,direction.z,0); }

			inline auto get_bbox_type() const -> BBoxType { return bbox_type; }
			inline auto get_x_radius() const -> float { return x_radius; }
			inline auto get_height() const -> float { return height; }
			inline auto get_z_radius() const -> float { return z_radius; }
		protected:
			inline auto get_transform() const -> glm::mat4 {
				return translation * rotation * scaling * base_translate;
			}
			inline auto get_translation() const -> glm::mat4 {
				return translation;
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

	//its inside entity that the draw call happens
	//it passes the parameters to the gpu
	//it has to hold a reference to:
	//	- the gpu program
	//	that acesses the Geometries
	/*		- the scaling mtx
	*		- the rotation mtx
	*		- the translation mtx */
	//	- the Mesh it renders
	class Entity : public Geometry{
		public:
			//full constructor
			Entity(std::shared_ptr<render::GPUprogram> gpu_program,
				std::shared_ptr<render::GPUprogram> wire_renderer,
				std::shared_ptr<render::Mesh> mesh,
				std::shared_ptr<render::WireMesh> wire_mesh,
				glm::vec4 cords, glm::vec4 direction, float speed,
				float x_angle, float y_angle, float z_angle,
				float x_scale, float y_scale, float z_scale,
				float x_radius, float z_radius, float height, BBoxType bbox_type
			);
			//simpler 
			Entity(std::shared_ptr<render::GPUprogram> gpu_program,
				std::shared_ptr<render::Mesh> mesh,
				glm::vec4 cords
			);
 
			auto draw() -> void;
			auto draw_wire() -> void;
		private:
			std::shared_ptr<render::GPUprogram> gpu_program;
			std::shared_ptr<render::GPUprogram> wire_renderer;
			std::shared_ptr<render::Mesh> mesh;
			std::shared_ptr<render::WireMesh> wire_mesh;
	};


	//TODO: make entities Player and Enemy

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