#include <cmath>
#include <exception>
#include <stdexcept>

#include "entities.hpp"

namespace entity{
	/*****************************
		Geometry implementation
	******************************/
	Geometry::Geometry(
		glm::vec4 cords, glm::vec4 direction,
		float x_angle, float y_angle, float z_angle,
		float x_scale, float y_scale, float z_scale,
		float x_radius, float z_radius, float height
		):
	cords(cords), direction(direction),
	x_angle(x_angle), y_angle(y_angle), z_angle(z_angle),
	x_scale(x_scale), y_scale(y_scale), z_scale(z_scale),
	x_radius(x_radius), z_radius(z_radius), height(height) {
		set_rotation();
		set_translation();
		set_scaling();
	}
	//simpler constructor with default values
	Geometry::Geometry(glm::vec4 cords):
	cords(cords), direction(glm::vec4(1.0f,0.0f,0.0f,0.0f)),
	x_angle(0), y_angle(0), z_angle(0),
	x_scale(1), y_scale(1), z_scale(1),
	x_radius(1), z_radius(1), height(1) {
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
		direction = rotation * direction;
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
		std::shared_ptr<render::GPUprogram> gpu_program_wire_mesh,
		std::shared_ptr<render::WireMesh> wire_mesh,
		glm::vec4 cords, glm::vec4 direction,
		float x_angle, float y_angle, float z_angle,
		float x_scale, float y_scale, float z_scale,
		float x_radius, float z_radius, float height):
		Geometry( cords, direction,
				x_angle, y_angle, z_angle,
				x_scale, y_scale, z_scale,
				x_radius,z_radius, height),
		gpu_program(gpu_program), mesh(mesh),
		gpu_program_wire_mesh(gpu_program_wire_mesh), wire_mesh(wire_mesh)
			{}
	Entity::Entity(std::shared_ptr<render::GPUprogram> gpu_program,
		std::shared_ptr<render::ObjMesh> mesh,
		glm::vec4 cords): Geometry(cords), gpu_program(gpu_program), mesh(mesh){}
	
	auto Entity::draw() -> void {
		gpu_program->set_uniform_value("model_scale",get_scaling_ptr());
		gpu_program->set_uniform_value("model_rotate",get_rotaion_ptr());
		gpu_program->set_uniform_value("model_translate",get_translation_ptr());

		mesh->draw();		
	}
	auto Entity::draw_wire() -> void {
		if(gpu_program_wire_mesh == nullptr || wire_mesh == nullptr){
			return;
		}
		gpu_program_wire_mesh->set_uniform_value("model_scale",get_scaling_ptr());
		gpu_program_wire_mesh->set_uniform_value("model_rotate",get_rotaion_ptr());
		gpu_program_wire_mesh->set_uniform_value("model_translate",get_translation_ptr());

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
		
		auto view_vec = point_look_at - point_c;
		view = mtx::cam_view(point_c,view_vec,up_vec);

		if(is_perspective){
			projection = mtx::perspective(3.141592f / 3.0f, 1.0f, -0.1f,-10.0f);
		}else{
			std::throw_with_nested(std::invalid_argument("Não implementei para ortográfico, LOL"));
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