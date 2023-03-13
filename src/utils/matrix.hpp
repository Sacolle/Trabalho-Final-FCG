#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

namespace mtx{
	auto indentity() -> glm::mat4;
	auto translate(float tx, float ty, float tz) -> glm::mat4;
	auto scale(float sx, float sy, float sz) -> glm::mat4;
	auto rot_x(float angle) -> glm::mat4;
	auto rot_y(float angle) -> glm::mat4;
	auto rot_z(float angle) -> glm::mat4;
	auto norm(glm::vec4 v) -> float;
	auto norm(glm::vec3 v) -> float;
	auto normalize(glm::vec4 vec) -> glm::vec4;
	auto rotate_rodriguez(float angle, glm::vec4 axis) -> glm::mat4;
	auto cross_prod(glm::vec4 u, glm::vec4 v) -> glm::vec4;
	//throws errors
	auto dot_prod(glm::vec4 u, glm::vec4 v) -> float;
	
	auto cam_view(glm::vec4 pos_c, glm::vec4 view_vec, glm::vec4 up_vec) -> glm::mat4;
	auto orthographic(float l,float r,float b,float t,float n,float f) -> glm::mat4;
	auto perspective(float fov,float aspect,float n,float f) -> glm::mat4;

	auto print(glm::mat4 mtx) -> void;
}
