#include <cmath>
#include <exception>
#include <stdexcept>

#include "entities.hpp"
#include "matrix.hpp"

namespace entity{
	/*****************************
		Geometry implementation
	******************************/
	Geometry::Geometry(glm::vec4 cords, glm::vec4 direction,
		float x_angle, float y_angle, float z_angle,
		float x_scale, float y_scale, float z_scale,
		float x_radius, float z_radius, float height):
	cords(cords), direction(direction),
	x_angle(x_angle), y_angle(y_angle), z_angle(z_angle),
	x_scale(x_scale), y_scale(y_scale), z_scale(z_scale),
	x_radius(x_radius), z_radius(z_radius), height(height) {
		set_rotation();
		set_translation();
		set_scaling();
	}

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
		//radius = 1
	}
	auto Geometry::translate_direction(glm::vec4 direction, float speed) -> void{
		glm::vec3 normalized_direction = direction / mtx::norm(direction);
		cords += normalized_direction * speed;
	}
	auto Geometry::rotate_increment(float x_axis, float y_axis, float z_axis) -> void{

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