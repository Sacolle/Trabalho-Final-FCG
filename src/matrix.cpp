#include "matrix.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <exception>
#include <stdexcept>
#include <string>
#include <sstream>
#include <iostream>
 
namespace mtx{
	inline auto matrix(
		float m00, float m01, float m02, float m03, // LINHA 1
		float m10, float m11, float m12, float m13, // LINHA 2
		float m20, float m21, float m22, float m23, // LINHA 3
		float m30, float m31, float m32, float m33  // LINHA 4
	) -> glm::mat4
	{
		return glm::mat4(
			m00, m10, m20, m30, // COLUNA 1
			m01, m11, m21, m31, // COLUNA 2
			m02, m12, m22, m32, // COLUNA 3
			m03, m13, m23, m33  // COLUNA 4
		);
	}
	auto indentity() -> glm::mat4{
		return matrix(
			1.0f , 0.0f , 0.0f , 0.0f , // LINHA 1
			0.0f , 1.0f , 0.0f , 0.0f , // LINHA 2
			0.0f , 0.0f , 1.0f , 0.0f , // LINHA 3
			0.0f , 0.0f , 0.0f , 1.0f   // LINHA 4
		);
	}
	auto translate(float tx, float ty, float tz) -> glm::mat4{
		return matrix(
			1.0f , 0.0f , 0.0f , tx ,  // LINHA 1
			0.0f , 1.0f , 0.0f , ty ,  // LINHA 2
			0.0f , 0.0f , 1.0f , tz ,  // LINHA 3
			0.0f , 0.0f , 0.0f , 1.0f  // LINHA 4
		);
	}
	auto scale(float sx, float sy, float sz) -> glm::mat4{
		return matrix(
			 sx  , 0.0f , 0.0f , 0.0f ,  // LINHA 1
			0.0f ,  sy  , 0.0f , 0.0f ,  // LINHA 2
			0.0f , 0.0f ,  sz  , 0.0f ,  // LINHA 3
			0.0f , 0.0f , 0.0f , 1.0f    // LINHA 4
		);
	}
	auto rot_x(float angle) -> glm::mat4{
		float c = cos(angle);
		float s = sin(angle);
		return matrix(
			1.0f , 0.0f , 0.0f , 0.0f ,  // LINHA 1
			0.0f ,   c  ,  -s  , 0.0f ,  // LINHA 2
			0.0f ,   s  ,   c  , 0.0f ,  // LINHA 3
			0.0f , 0.0f , 0.0f , 1.0f    // LINHA 4
		);
	}
	auto rot_y(float angle) -> glm::mat4{
		float c = cos(angle);
		float s = sin(angle);
		return matrix(
			  c  , 0.0f ,   s  , 0.0f ,  // LINHA 1
			0.0f , 1.0f , 0.0f , 0.0f ,  // LINHA 2
			 -s  , 0.0f ,   c  , 0.0f ,  // LINHA 3
			0.0f , 0.0f , 0.0f , 1.0f    // LINHA 4
		);
	}
	auto rot_z(float angle) -> glm::mat4 {
		float c = cos(angle);
		float s = sin(angle);
		return matrix(
			c  ,  -s  , 0.0f , 0.0f ,  // LINHA 1
			s  ,   c  , 0.0f , 0.0f ,  // LINHA 2
			0.0f , 0.0f , 1.0f , 0.0f ,  // LINHA 3
			0.0f , 0.0f , 0.0f , 1.0f    // LINHA 4
		);
	}
	auto norm(glm::vec4 v) -> float {
		float vx = v.x;
		float vy = v.y;
		float vz = v.z;

		return sqrt( vx*vx + vy*vy + vz*vz );
	}
	auto norm(glm::vec3 v) -> float {
		float vx = v.x;
		float vy = v.y;
		float vz = v.z;

		return sqrt( vx*vx + vy*vy + vz*vz );
	}
	auto rotate_rodriguez(float angle, glm::vec4 axis) -> glm::mat4 {
		float c = cos(angle);
    	float s = sin(angle);

    	glm::vec4 v = axis / norm(axis);

    	float vx = v.x;
    	float vy = v.y;
    	float vz = v.z;

    	return glm::mat4(
        	vx*vx*(1-c) + c    , vx*vy*(1-c) - vz*s , vx*vz*(1-c) + vy*s , 0.0f ,  // LINHA 1
        	vx*vy*(1-c) + vz*s , vy*vy*(1-c) + c    , vy*vz*(1-c) - vx*s , 0.0f ,  // LINHA 2
        	vx*vz*(1-c) - vy*s , vy*vz*(1-c) + vx*s , vz*vz*(1-c) + c    , 0.0f ,  // LINHA 3
            	  0.0f         ,       0.0f         ,       0.0f         , 1.0f    // LINHA 4
    	);
	}
	auto cross_prod(glm::vec4 u, glm::vec4 v) -> glm::vec4 {
		float u1 = u.x;
		float u2 = u.y;
		float u3 = u.z;
		float v1 = v.x;
		float v2 = v.y;
		float v3 = v.z;

		return glm::vec4(
			u2*v3 - u3*v2, // Primeiro coeficiente
			u3*v1 - u1*v3, // Segundo coeficiente
			u1*v2 - u2*v1, // Terceiro coeficiente
			0.0f 		   // w = 0 para vetores.
		);
	}
	//throws errors
	auto dot_prod(glm::vec4 u, glm::vec4 v) -> float {
		float u1 = u.x, u2 = u.y, u3 = u.z, u4 = u.w;
		float v1 = v.x, v2 = v.y, v3 = v.z, v4 = v.w;

		if ( u4 != 0.0f || v4 != 0.0f )
		{
			std::stringstream error_stream;
			error_stream << "Dot product of points U.w = ";
			error_stream << u.w;
			error_stream << " and V.w = ";
			error_stream << v.w;
			std::throw_with_nested( std::invalid_argument(error_stream.str()));
		}

		return u1*v1 + u2*v2 + u3*v3;
	}
	
	auto cam_view(glm::vec4 pos_c, glm::vec4 view_vec, glm::vec4 up_vec) -> glm::mat4 {

		glm::vec4 w = -view_vec;
		glm::vec4 u = cross_prod(up_vec,w);
		// Normalizamos os vetores u e w
		w = w / norm(w);
		u = u / norm(u);

		//printf("\n %f %f %f %f\n", view_vec.x, view_vec.y, view_vec.z, view_vec.w);
		//printf("\n %f %f %f %f\n", up_vec.x, up_vec.y, up_vec.z, up_vec.w);
		//printf("\n %f %f %f %f\n", u.x, u.y, u.z, u.w);

		glm::vec4 v = cross_prod(w,u);

		glm::vec4 origin_o = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		float ux = u.x, uy = u.y, uz = u.z;
		float vx = v.x, vy = v.y, vz = v.z;
		float wx = w.x, wy = w.y, wz = w.z;

		return matrix(
			ux  ,  uy  ,  uz  , dot_prod(-u,pos_c - origin_o) ,  // LINHA 1
			vx  ,  vy  ,  vz  , dot_prod(-v,pos_c - origin_o) ,  // LINHA 2
			wx  ,  wy  ,  wz  , dot_prod(-w,pos_c - origin_o) ,  // LINHA 3
			0.0f,  0.0f,  0.0f, 1.0f                             // LINHA 4
		);
	}
	auto orthographic(float l, float r, float b, float t, float n, float f) -> glm::mat4 {
		return matrix(
			2.0f/(r - l) , 0.0f         , 0.0f         , -(r + l)/(r - l) ,  // LINHA 1
			0.0f         , 2.0f/(t - b) , 0.0f         , -(t + b)/(t - b) ,  // LINHA 2
			0.0f         , 0.0f         , 2.0f/(f - n) , -(f + n)/(f - n) ,  // LINHA 3
			0.0f         , 0.0f         , 0.0f         , 1.0f    // LINHA 4
		);
	}
	auto perspective(float fov, float aspect, float n, float f) -> glm::mat4 {
		float t = fabs(n) * tanf(fov / 2.0f);
		float b = -t;
		float r = t * aspect;
		float l = -r;

		glm::mat4 P = matrix(
			n    , 0.0f , 0.0f , 0.0f ,  // LINHA 1
			0.0f ,   n  , 0.0f , 0.0f ,  // LINHA 2
			0.0f , 0.0f ,  n+f , -n*f ,  // LINHA 3
			0.0f , 0.0f , 1.0f , 0.0f    // LINHA 4
		);
		glm::mat4 M = orthographic(l, r, b, t, n, f);

		return -M*P;
	}
	auto print(glm::mat4 mtx) -> void {
		for(int i = 0; i < 4; i++){
			for(int j = 0; j < 4; j++){
				if(j == 3)
					std::cout << mtx[j][i];
				else
					std::cout << mtx[j][i] << ", ";
			}
			std::cout << std::endl;
		}
	}
}