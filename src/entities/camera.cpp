
#include "camera.hpp"

namespace entity{
	/**************************
		Camera implementation
	***************************/
	Camera::Camera(): point_look_at(glm::vec4(0.0f,0.0f,0.0f,1.0f)),
		up_vec(glm::vec4(0.0f,1.0f,0.0f,0.0f)),
		aspect_ratio(1.0f), near_plane(0.1f), far_plane(20.0f), cam_speed(0.2f)
	{
		const auto point_c = glm::vec4(0.0f,0.0f,1.0f,1.0f);
		camera_direction = mtx::normalize(point_look_at - point_c);
		view = mtx::cam_view(point_c,camera_direction,up_vec);

		projection = mtx::perspective(3.141592f / 3.0f, aspect_ratio, -near_plane,-far_plane);
	}
	auto Camera::update_aspect_ratio(float new_aspect_ratio) -> void {
		#define SIGMA 0.0001f
		if(abs(new_aspect_ratio - aspect_ratio) > SIGMA){
			projection = mtx::perspective(3.141592f / 3.0f, new_aspect_ratio, -near_plane,-far_plane);
			aspect_ratio = new_aspect_ratio;
		}
	}
	auto Camera::update_position(LookAtParameters &parameters) -> void{
        float y = parameters.radius*sin(parameters.phi);
        float z = parameters.radius*cos(parameters.phi)*cos(parameters.theta);
        float x = parameters.radius*cos(parameters.phi)*sin(parameters.theta);

		camera_position = glm::vec4(x,y,z,1.0f);
		camera_direction = mtx::normalize(point_look_at - camera_position);

		view = mtx::cam_view(camera_position,camera_direction,up_vec);
	}
	auto Camera::update_position(entity::PressedKeys &keys, float delta_time) -> void{
		float pressed__directions = sum_direction(keys);
		float distance = TOTAL_DIRECTIONS/pressed__directions * cam_speed * delta_time;
		
		if(keys.w){
			camera_position = camera_position + camera_direction*distance;
		}
		if(keys.s){
			camera_position = camera_position - camera_direction*distance;
		}
		if(keys.d){
			camera_position = camera_position + mtx::cross_prod(camera_direction,up_vec)*distance;
		}
		if(keys.a){
			camera_position = camera_position - mtx::cross_prod(camera_direction,up_vec)*distance;
		}

		view = mtx::cam_view(camera_position,camera_direction,up_vec);
	}
	auto Camera::sum_direction(entity::PressedKeys &keys) -> float {
		float count = 0;
		if(keys.w){
			count++;
		}
		if(keys.s){
			count++;
		}
		if(keys.d){
			count++;
		}
		if(keys.a){
			count++;
		}
		return count;
	}
	auto Camera::update_view(RotationAngles &angles) -> void {
		if(angles.angleX == 0 && angles.angleZ == 0) return;

		// ROTAÇÃO VERTICAL
    	glm::vec4 lado = mtx::cross_prod(up_vec, camera_direction); // Calcula o lado, para rotacionar verticalmente
    	glm::vec4 aux = camera_direction * mtx::rotate_rodriguez(angles.angleZ, lado);   // Rotação no eixo lado (vertical)

    	// TRAVA DA ROTAÇÃO VERTICAL
    	if(mtx::dot_prod(lado, mtx::cross_prod(up_vec, aux)) > 0) // Testa se o novo valor de lado é igual ao antigo
        	camera_direction = aux;                                 // Caso seja, salva o novo camera_view (permite a rotação)

    	// ROTAÇÃO HORIZONTAL
    	camera_direction = mtx::normalize(camera_direction * mtx::rotate_rodriguez(-angles.angleX, up_vec)); // Rotação no eixo up (horizontal)

		angles.angleX = 0.0f;
		angles.angleZ = 0.0f;

		view = mtx::cam_view(camera_position,camera_direction,up_vec);
	}
}