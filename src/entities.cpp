#include <cmath>
#include <exception>
#include <stdexcept>
#include <iostream>

#include "entities.hpp"

#define SIGMA 0.0001f

namespace entity{
	/*****************************
		Geometry implementation
	******************************/
	Geometry::Geometry(
		glm::vec4 cords, glm::vec4 direction,
		float x_angle, float y_angle, float z_angle,
		float x_scale, float y_scale, float z_scale,
		float x_radius, float z_radius, float height, BBoxType bbox_type
		):
	cords(cords), direction(direction),
	x_angle(x_angle), y_angle(y_angle), z_angle(z_angle),
	x_scale(x_scale), y_scale(y_scale), z_scale(z_scale),
	x_radius(x_radius), z_radius(z_radius), height(height), bbox_type(bbox_type) {
		base_direction = direction;
		set_rotation();
		set_translation();
		set_scaling();
	}
	//simpler constructor with default values
	Geometry::Geometry(glm::vec4 cords):
	cords(cords), direction(glm::vec4(1.0f,0.0f,0.0f,0.0f)), base_direction(glm::vec4(1.0f,0.0f,0.0f,0.0f)),
	x_angle(0), y_angle(0), z_angle(0),
	x_scale(1), y_scale(1), z_scale(1),
	x_radius(1), z_radius(1), height(1), bbox_type(BBoxType::Rectangle) {
		set_rotation();
		set_translation();
		set_scaling();
	}

	Geometry::~Geometry(){

	}
	//normalize the vectors
	auto Geometry::translate_foward(float speed) -> void{
		cords += direction * speed;
		set_translation();
	}
	auto Geometry::translate_direction(glm::vec4 direction, float speed) -> void{
		glm::vec4 normalized_direction = direction / mtx::norm(direction);
		cords += normalized_direction * speed;
		set_translation();
	}
	auto Geometry::rotate_increment(float x, float y, float z) -> void{
		x_angle += x;
		y_angle += y;
		z_angle += z;
		set_rotation();
		direction = rotation * base_direction;
	}
	auto Geometry::set_cords(float x, float y, float z) -> void {
		cords.x = x;
		cords.y = y;
		cords.z = z;
		set_translation();
	}
	auto Geometry::set_angles(float x, float y, float z) -> void {
		x_angle = x;
		y_angle = y;
		z_angle = z;
		set_rotation();
		direction = rotation * base_direction;
	}
	auto Geometry::set_y_angle(float y) -> void {
		y_angle = y;
		set_rotation();
		direction = rotation * base_direction;
	}
	/*
	//TODO: implementar com shared_ptr
	auto Geometry::in_2d_bounds(Geometry &geometry) -> bool{
		return false;
	}
	auto Geometry::in_3d_bounds(Geometry &geometry) -> bool{
		return false;
	}*/
	/**************************
		Entity implementation
	***************************/
	Entity::Entity(std::shared_ptr<render::GPUprogram> gpu_program,
		std::shared_ptr<render::ObjMesh> mesh,
		std::shared_ptr<render::WireMesh> wire_mesh,
		glm::vec4 cords, glm::vec4 direction,
		float x_angle, float y_angle, float z_angle,
		float x_scale, float y_scale, float z_scale,
		float x_radius, float z_radius, float height, BBoxType bbox_type):
		Geometry( cords, direction,
				x_angle, y_angle, z_angle,
				x_scale, y_scale, z_scale,
				x_radius,z_radius, height, bbox_type),
		gpu_program(gpu_program), mesh(mesh), wire_mesh(wire_mesh)
			{
				set_base_translate(0,0,0);
			}
	Entity::Entity(std::shared_ptr<render::GPUprogram> gpu_program,
		std::shared_ptr<render::ObjMesh> mesh,
		glm::vec4 cords): Geometry(cords), gpu_program(gpu_program), mesh(mesh){}
	
	auto Entity::draw() -> void {

		gpu_program->set_uniform_mtx("model_scale",get_scaling_ptr());
		gpu_program->set_uniform_mtx("model_rotate",get_rotaion_ptr());
		gpu_program->set_uniform_mtx("model_translate",get_translation_ptr());
		
		gpu_program->set_uniform_bool("render_wire_frame", false);
		gpu_program->set_uniform_mtx("model_base_translate",get_base_translate_ptr());
		
		mesh->draw();		
	}
	auto Entity::draw_wire() -> void {
		if(wire_mesh == nullptr){
			return;
		}
		gpu_program->set_uniform_mtx("model_scale",get_scaling_ptr());
		gpu_program->set_uniform_mtx("model_rotate",get_rotaion_ptr());
		gpu_program->set_uniform_mtx("model_translate",get_translation_ptr());
		//values for wire frame
		gpu_program->set_uniform_bool("render_wire_frame", true);
		gpu_program->set_uniform_mtx("bbox_scale", glm::value_ptr(mtx::scale(x_radius, height, z_radius)));

		wire_mesh->draw();		
	}
	/**************************
		Camera implementation
	***************************/
	Camera::Camera(bool is_perspective){
		//tem q se incializar o point_c com um valor não nulo para pelo menos uma das coords além de w
		point_c = glm::vec4(0.0f,0.0f,1.0f,1.0f);
		point_look_at = glm::vec4(0.0f,0.0f,0.0f,1.0f);
		up_vec = glm::vec4(0.0f,1.0f,0.0f,0.0f);
		aspect_ratio = 1.0f;
		
		auto view_vec = point_look_at - point_c;
		view = mtx::cam_view(point_c,view_vec,up_vec);

		if(is_perspective){
			projection = mtx::perspective(3.141592f / 3.0f, aspect_ratio, -0.1f,-10.0f);
		}else{
			std::throw_with_nested(std::invalid_argument("Não implementei para ortográfico, LOL"));
		}
	}

	auto Camera::update_aspect_ratio(float new_aspect_ratio) -> void {
		if(abs(new_aspect_ratio - aspect_ratio) > SIGMA){
			projection = mtx::perspective(3.141592f / 3.0f, new_aspect_ratio, -0.1f,-10.0f);
			aspect_ratio = new_aspect_ratio;
		}
	}

	auto Camera::update_position(float phi, float theta, float radius) -> void{
        float y = radius*sin(phi);
        float z = radius*cos(phi)*cos(theta);
        float x = radius*cos(phi)*sin(theta);

		point_c = glm::vec4(x,y,z,1.0f);
		auto view_vec = point_look_at - point_c;

		view = mtx::cam_view(point_c,view_vec,up_vec);
	}
}