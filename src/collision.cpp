#include "collision.hpp"

#include <cmath>

namespace utils{
	SpacialHash::SpacialHash(float cell_x, float cell_y, float cell_z):
		cell_x(cell_x), cell_y(cell_y), cell_z(cell_z){}

	//insert the shared_ptrs at every cell the entity ocupies,
	//as cords(x, y) + it's bounding box
	//return the num of cells it ocupies
	auto SpacialHash::insert(std::shared_ptr<entity::Entity> elem) -> int {
		int center_x = static_cast<int>(floorf(elem->get_x_cords()/cell_x));
		int center_y = static_cast<int>(floorf(elem->get_y_cords()/cell_y));
		int center_z = static_cast<int>(floorf(elem->get_z_cords()/cell_z));

		auto key = std::make_tuple(center_x,center_y,center_x);

		//does not cointain the key, therefore, has to initialize the unordered_map
		if(!map.count(key)){
			map[key] = std::unordered_set<std::shared_ptr<entity::Entity>>();
		}
		map[key].insert(elem);

		//loop over the bounds

	}
	
	//remove the shared_ptr at the center,
	//then at every other cell it also ocupies (x, y) + bbox
	//return the num of cells it removed from
	auto SpacialHash::remove(std::shared_ptr<entity::Entity> elem) -> int {

	}

	//return a set of all the entities nearby
	auto SpacialHash::get_neighbors(std::shared_ptr<entity::Entity> elem) -> std::unordered_set<std::shared_ptr<entity::Entity>>{

	}
}