#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include "matrix.hpp"

namespace utils {
	class Animation {
		public:
			Animation(float d): duration(d){};
			virtual auto get_transform(float instant) -> glm::mat4{
				return mtx::indentity();
			};
		protected:
			float duration;
	};

	class Bezier3 : public Animation {
		public:
			Bezier3(float duration, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3
			): Animation(duration), p0(p0), p1(p1), p2(p2), p3(p3){
				assert(duration != 0);
			}
			
			auto get_transform(float instant) -> glm::mat4 override;
		private:
			auto bezier_curve(float t) -> glm::vec3;
			inline auto bezier(float t, glm::vec3 a, glm::vec3 b) -> glm::vec3 {
				return a + t * (b - a);
			}
			glm::vec3 p0;
			glm::vec3 p1;
			glm::vec3 p2;
			glm::vec3 p3;
	};

}