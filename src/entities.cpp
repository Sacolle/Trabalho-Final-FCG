#include <cmath>
#include <exception>
#include <stdexcept>

#include "entities.hpp"
#include "matrix.hpp"

namespace entity{
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