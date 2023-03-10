#include <cmath>
#include <exception>
#include <stdexcept>
#include <iostream>
#include <utility>

#include "entities.hpp"
#include "matrix.hpp"

#define SIGMA 0.0001f

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
	auto Geometry::translate_direction(glm::vec4 direction) -> void{
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

	auto Geometry::box_to_box_collision(std::shared_ptr<Geometry> geometry) const -> bool{
		const auto future_pos = cords + (direction * speed);

		const auto geo_cords = geometry->get_cords();
		const float box_x = geometry->get_x_radius();
		const float box_z = geometry->get_z_radius();

		//solution taken from stackoverflow https://stackoverflow.com/a/62852710
		return std::max(geo_cords.x - box_x, future_pos.x - x_radius) < std::min(geo_cords.x + box_x, future_pos.x + x_radius) &&
		std::max(geo_cords.z - box_z, future_pos.z - z_radius) < std::min(geo_cords.z + box_z, future_pos.z + z_radius);
	}
	auto Geometry::cilinder_to_box_collision(std::shared_ptr<Geometry> geometry) const -> bool{
		const auto future_pos = cords + (direction * speed);
		//std::cout << "Center: " << future_pos.x << " " << future_pos.z << std::endl;
		
		const auto center2geometry = geometry->get_cords() - future_pos;

		const float co = abs(center2geometry.x);
		const float ca = abs(center2geometry.z);
		const float hipotenuse = sqrtf(co*co + ca*ca);
		
		const float rx = x_radius*cosf(ca/hipotenuse);
		const float rz = z_radius*sinf(co/hipotenuse);

		//std::cout << "cos(" << ca << "/" << hipotenuse << ") = " << cosf(ca/hipotenuse) << std::endl;
		//std::cout << "rx: " << rx << " = " << x_radius << "*" << cosf(ca/hipotenuse) << std::endl;

		auto boundry_point = future_pos;
		boundry_point[3] = 1;
		if(center2geometry.x > 0){
			boundry_point[0] += rx;
		}else{
			boundry_point[0] -= rx;
		}
		if(center2geometry.z > 0){
			boundry_point[2] += rz;
		}else{
			boundry_point[2] -= rz;
		}
		//std::cout << "Boundry Point: " << boundry_point.x << " " << boundry_point.z << std::endl;

		const auto geo_cords = geometry->get_cords();
		const float box_x = geometry->get_x_radius();
		const float box_z = geometry->get_z_radius();

		bool x_overlap = value_in_range(boundry_point.x, geo_cords.x - box_x, geo_cords.x + box_x);
		bool z_overlap = value_in_range(boundry_point.z, geo_cords.z - box_z, geo_cords.z + box_z);
		return x_overlap && z_overlap;
	}
	auto Geometry::box_to_cilinder_collision(std::shared_ptr<Geometry> geometry) const -> bool{
		const auto future_pos = cords + (direction * speed);
		const auto geometry2center = future_pos - geometry->get_cords();

		const float co = abs(geometry2center.x);
		const float ca = abs(geometry2center.z);
		const float hipotenuse = sqrtf(co*co + ca*ca);
		
		const float rx = x_radius*cosf(ca/hipotenuse);
		const float rz = z_radius*sinf(co/hipotenuse);

		auto boundry_point = geometry2center;
		boundry_point[3] = 1;
		if(geometry2center.x > 0){
			boundry_point[0] += rx;
		}else{
			boundry_point[0] -= rx;
		}
		if(geometry2center.z > 0){
			boundry_point[2] += rz;
		}else{
			boundry_point[2] -= rz;
		}
		bool x_overlap = value_in_range(boundry_point.x, future_pos.x - x_radius, future_pos.x + x_radius);
		bool z_overlap = value_in_range(boundry_point.z, future_pos.z - z_radius, future_pos.z + z_radius);
		return x_overlap && z_overlap;
	}
	auto Geometry::cilinder_to_cilinder_collision(std::shared_ptr<Geometry> geometry) const -> bool{
		const auto future_pos = cords + (direction * speed);

		const auto center2geometry = geometry->get_cords() - future_pos;

		const float co = abs(center2geometry.x);
		const float ca = abs(center2geometry.z);
		const float hipotenuse = sqrtf(co*co + ca*ca);
		
		const float rx = x_radius*cosf(ca/hipotenuse);
		const float rz = z_radius*sinf(co/hipotenuse);

		auto boundry_point = center2geometry;
		boundry_point[3] = 1;
		if(center2geometry.x > 0){
			boundry_point[0] += rx;
		}else{
			boundry_point[0] -= rx;
		}
		if(center2geometry.z > 0){
			boundry_point[2] += rz;
		}else{
			boundry_point[2] -= rz;
		}

		const auto geometry2center = future_pos - geometry->get_cords();
		
		const float geo_rx = geometry->get_x_radius()*cosf(ca/hipotenuse);
		const float geo_rz = geometry->get_z_radius()*sinf(co/hipotenuse);

		bool right_x = geometry2center.x > 0;
		bool right_z = geometry2center.z > 0;

		//FIXME: acho q se geometry2center for 0 vai dar merda	
		return (right_x && (geometry2center.x + geo_rx > boundry_point.x)) ||
			(!right_x && (geometry2center.x - geo_rx < boundry_point.x));

		/*
		if(geometry2center.x > 0){
			if(geometry2center.x + geo_rx > boundry_point.x){
				return true;
			}
		}else{
			if(geometry2center.x - geo_rx < boundry_point.x){
				return true;
			}
		}
		if(geometry2center.z > 0){
			if(geometry2center.z + geo_rz > boundry_point.z){
				return true;
			}
		}else{
			if(geometry2center.z - geo_rz < boundry_point.z){
				return true;
			}
		}*/
	}

	//TODO: change ocurrencers of cords to future_pos
	auto Geometry::foward_will_collide(std::shared_ptr<Geometry> geometry) -> bool {
		//std::cout << "collide test" << std::endl;
		auto future_pos = cords + (direction * speed);
		if(bbox_type == BBoxType::Rectangle){
			if(geometry->get_bbox_type() == BBoxType::Rectangle){
				return box_to_box_collision(geometry);
			}else{
				return box_to_cilinder_collision(geometry);
			}
		}else{ 
			if(geometry->get_bbox_type() == BBoxType::Rectangle){
				return cilinder_to_box_collision(geometry);
			}else{
				return cilinder_to_cilinder_collision(geometry);
			}
		}
	}
	/*
	auto Geometry::in_3d_bounds(Geometry &geometry) -> bool{
		return false;
	}*/
	/**************************
		Entity implementation
	***************************/
	Entity::Entity(std::shared_ptr<render::GPUprogram> gpu_program,
		std::shared_ptr<render::ObjMesh> mesh,
		std::shared_ptr<render::WireMesh> wire_mesh,
		glm::vec4 cords, glm::vec4 direction, float speed,
		float x_angle, float y_angle, float z_angle,
		float x_scale, float y_scale, float z_scale,
		float x_radius, float z_radius, float height, BBoxType bbox_type):
		Geometry( cords, direction, speed,
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
		view_vec = glm::normalize(point_look_at - point_c);
		aspect_ratio = 1.0f;
		
		auto view_vec = point_look_at - point_c;
		view = mtx::cam_view(point_c,view_vec,up_vec);

		if(is_perspective){
			projection = mtx::perspective(3.141592f / 3.0f, aspect_ratio, -0.1f,-20.0f);
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
		view_vec = glm::normalize(point_look_at - point_c);

		view = mtx::cam_view(point_c,view_vec,up_vec);
	}

	auto Camera::update_position(Direction dir) -> void{
		float velocity = 0.2;
		if(dir == Stay) {
			return;
		}
		if(dir == Front){
			point_c = point_c + view_vec*velocity;
		}
		if(dir == Back){
			point_c = point_c - view_vec*velocity;
		}
		if(dir == Right){
			point_c = point_c + mtx::cross_prod(view_vec,up_vec)*velocity;
		}
		if(dir == Left){
			point_c = point_c - mtx::cross_prod(view_vec,up_vec)*velocity;
		}

		view = mtx::cam_view(point_c,view_vec,up_vec);
	}
}