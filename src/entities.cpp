#include <cmath>
#include <exception>
#include <stdexcept>
#include <iostream>
#include <utility>

#include "entities.hpp"


namespace entity{
	inline bool value_in_range(int value, int min, int max){
		return (value >= min) && (value <= max);
	}
	/*****************************
		Geometry implementation
	******************************/
	Geometry::Geometry(
		glm::vec4 cords, glm::vec4 direction, float speed,
		float x_angle, float y_angle, float z_angle,
		float x_scale, float y_scale, float z_scale,
		float x_radius, float z_radius, float height, BBoxType bbox_type
		):
	cords(cords), direction(direction), speed(speed),
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
	cords(cords), direction(glm::vec4(1.0f,0.0f,0.0f,0.0f)), speed(0.01f),
	base_direction(glm::vec4(1.0f,0.0f,0.0f,0.0f)),
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
	auto Geometry::translate_foward() -> void{
		cords += direction * speed;
		set_translation();
	}
	auto Geometry::translate_direction(const glm::vec4 dir) -> void{
		cords += dir * speed;
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
	/**************************
		Entity implementation
	***************************/
	Entity::Entity(std::shared_ptr<render::GPUprogram> gpu_program,
		std::shared_ptr<render::GPUprogram> wire_renderer,
		std::shared_ptr<render::Mesh> mesh,
		std::shared_ptr<render::WireMesh> wire_mesh,
		glm::vec4 cords, glm::vec4 direction, float speed,
		float x_angle, float y_angle, float z_angle,
		float x_scale, float y_scale, float z_scale,
		float x_radius, float z_radius, float height, BBoxType bbox_type):
		Geometry( cords, direction, speed,
				x_angle, y_angle, z_angle,
				x_scale, y_scale, z_scale,
				x_radius,z_radius, height, bbox_type),
		gpu_program(gpu_program), wire_renderer(wire_renderer), mesh(mesh), wire_mesh(wire_mesh)
		{
			set_base_translate(0,0,0);
		}
	Entity::Entity(std::shared_ptr<render::GPUprogram> gpu_program,
		std::shared_ptr<render::Mesh> mesh,
		glm::vec4 cords): Geometry(cords), gpu_program(gpu_program), mesh(mesh){}
	
	auto Entity::draw() -> void {
		const auto transform = get_transform();
		gpu_program->set_mtx("model_transform",glm::value_ptr(transform));
		mesh->draw(gpu_program);		
	}
	auto Entity::draw_wire() -> void {
		if(wire_mesh == nullptr || wire_renderer == nullptr){
			return;
		}
		const auto transform = get_translation() * mtx::scale(get_x_radius(), get_height(), get_z_radius());
		wire_renderer->set_mtx("transform", glm::value_ptr(transform));

		wire_mesh->draw();		
	} 
	/**************************
		Camera implementation
	***************************/
	Camera::Camera(): point_look_at(glm::vec4(0.0f,0.0f,0.0f,1.0f)),
		up_vec(glm::vec4(0.0f,1.0f,0.0f,0.0f)),
		aspect_ratio(1.0f), near_plane(0.1f), far_plane(20.0f)
	{
		const auto point_c = glm::vec4(0.0f,0.0f,1.0f,1.0f);
		auto view_vec = point_look_at - point_c;
		view = mtx::cam_view(point_c,view_vec,up_vec);

		projection = mtx::perspective(3.141592f / 3.0f, aspect_ratio, -near_plane,-far_plane);
	}
	auto Camera::update_aspect_ratio(float new_aspect_ratio) -> void {
		#define SIGMA 0.0001f
		if(abs(new_aspect_ratio - aspect_ratio) > SIGMA){
			projection = mtx::perspective(3.141592f / 3.0f, new_aspect_ratio, -near_plane,-far_plane);
			aspect_ratio = new_aspect_ratio;
		}
	}
	auto Camera::update_position(float phi, float theta, float radius) -> void{
        float y = radius*sin(phi);
        float z = radius*cos(phi)*cos(theta);
        float x = radius*cos(phi)*sin(theta);

		const auto direction = glm::vec4(x,y,z,1.0f);
		auto view_vec = point_look_at - direction ;

		view = mtx::cam_view(direction,view_vec,up_vec);
	}
}