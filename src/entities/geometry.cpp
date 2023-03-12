#include <cmath>
#include <exception>
#include <stdexcept>
#include <iostream>
#include <utility>

#include "geometry.hpp"


namespace entity{
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
		set_base_translate(0,0,0);
	}

	Geometry::Geometry(glm::vec4 cords):
	cords(cords), direction(glm::vec4(1.0f,0.0f,0.0f,0.0f)), speed(0.01f),
	base_direction(glm::vec4(1.0f,0.0f,0.0f,0.0f)),
	x_angle(0), y_angle(0), z_angle(0),
	x_scale(1), y_scale(1), z_scale(1),
	x_radius(1), z_radius(1), height(1), bbox_type(BBoxType::Rectangle) {
		set_rotation();
		set_translation();
		set_scaling();
		set_base_translate(0,0,0);
	} 
	Geometry::Geometry():
	cords(glm::vec4(0.01f,0.01f,0.01f,0.0f)), direction(glm::vec4(1.0f,0.0f,0.0f,0.0f)), speed(0.01f),
	base_direction(glm::vec4(1.0f,0.0f,0.0f,0.0f)),
	x_angle(0), y_angle(0), z_angle(0),
	x_scale(1), y_scale(1), z_scale(1),
	x_radius(1), z_radius(1), height(1), bbox_type(BBoxType::Rectangle) {
		set_rotation();
		set_translation();
		set_scaling();
		set_base_translate(0,0,0);
	}
	Geometry::~Geometry(){

	}
	//normalize the vectors
	auto Geometry::translate_foward(const float delta_time) -> void{
		cords += direction * (speed * delta_time);
		set_translation();
	}
	auto Geometry::translate_direction(const glm::vec4 dir, const float delta_time) -> void{
		cords += dir * (speed * delta_time);
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
	auto Geometry::set_scale(float x, float y, float z) -> void {
		x_scale = x;
		y_scale = y;
		z_scale = z;
		set_scaling();
	}
	auto set_direction(glm::vec4 new_dir) -> void {
		//TODO: set the direction and get the angles
	}
}