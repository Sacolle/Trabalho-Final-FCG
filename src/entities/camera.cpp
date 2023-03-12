
#include "camera.hpp"

namespace entity{
	/**************************
		Camera implementation
	***************************/
	Camera::Camera(): point_look_at(glm::vec4(0.0f,0.0f,0.0f,1.0f)),
		up_vec(glm::vec4(0.0f,1.0f,0.0f,0.0f)),
		aspect_ratio(1.0f), near_plane(0.1f), far_plane(20.0f)
	{
		const auto point_c = glm::vec4(0.0f,0.0f,1.0f,1.0f);
		direction = point_look_at - point_c;
		view = mtx::cam_view(point_c,direction,up_vec);

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

		camera_position = glm::vec4(x,y,z,1.0f);
		direction = glm::normalize(point_look_at - camera_position);

		view = mtx::cam_view(camera_position,direction,up_vec);
	}
	auto Camera::update_position(Direction dir, int size, float delta_time) -> void{
		float velocity = (4/size) * 0.2f * delta_time;
		
		if(dir == Front){
			camera_position = camera_position + direction*velocity;
		}
		if(dir == Back){
			camera_position = camera_position - direction*velocity;
		}
		if(dir == Right){
			camera_position = camera_position + mtx::cross_prod(direction,up_vec)*velocity;
		}
		if(dir == Left){
			camera_position = camera_position - mtx::cross_prod(direction,up_vec)*velocity;
		}

		view = mtx::cam_view(camera_position,direction,up_vec);
	}
}