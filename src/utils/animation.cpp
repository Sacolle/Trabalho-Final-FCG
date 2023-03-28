#include "animation.hpp"

#include <cmath>
#include <iostream>


namespace utils {
	auto Bezier3::get_transform(float instant) -> glm::mat4 {
		const float t = fmodf(instant, duration) / duration;
		const auto p = bezier_curve(t);
		return mtx::translate(p.x, p.y, p.z);
	}
	auto Bezier3::bezier_curve(float t) -> glm::vec3 {
		const auto ab = bezier(t, p0, p1);
		const auto bc = bezier(t, p1, p2);
		const auto cd = bezier(t, p2, p3);
		
		const auto abc = bezier(t, ab, bc);
		const auto bcd = bezier(t, bc, cd);

		return bezier(t, abc, bcd);
	}
}