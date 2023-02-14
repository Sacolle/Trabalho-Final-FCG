#include "collision.hpp"

#include <cmath>

namespace utils{
	SpacialHash::SpacialHash(float cell_x, float cell_y):
		cell_x(cell_x), cell_y(cell_y){}


	auto SpacialHash::get_key(std::shared_ptr<entity::Entity> elem) -> std::pair<int,int>{
		int center_x = static_cast<int>(floorf(elem->get_x_cords()/cell_x));
		int center_y = static_cast<int>(floorf(elem->get_y_cords()/cell_y));

		return std::make_pair(center_x,center_y);
	}

	//insert the shared_ptrs at every cell the entity ocupies,
	//as cords(x, y) + it's bounding box
	//return the num of cells it ocupies
	auto SpacialHash::insert(std::shared_ptr<entity::Entity> elem) -> int {
		auto key = get_key(elem);
		//does not cointain the key, therefore, has to initialize the unordered_set
		if(!map.count(key)){
			std::unordered_set<std::shared_ptr<entity::Entity>> init;
			map[key] = std::move(init);
		}
		map[key].insert(elem);
		//TODO: Implement large objects that may be in in more than square
		return 1;
	}
	
	//remove the shared_ptr at the center,
	//then at every other cell it also ocupies (x, y) + bbox
	//return the num of cells it removed from
	auto SpacialHash::remove(std::shared_ptr<entity::Entity> elem) -> int {
		auto key = get_key(elem);
		//does not cointain the key, therefore the element is not on the map
		if(!map.count(key)){
			return 0;
		}
		map[key].erase(elem);
		//TODO: Implement large objects that may be in in more than square
		return 1;
	}

	//return a set of all the entities nearby
	auto SpacialHash::get_neighbors(std::shared_ptr<entity::Entity> elem)
		-> std::vector<std::unordered_set<std::shared_ptr<entity::Entity>>&>
	{
		auto key = get_key(elem);
		std::vector<std::unordered_set<std::shared_ptr<entity::Entity>>&> neighbors;

		if(map[key].size() > 1){
			neighbors.push_back(map[key]);
		}
		for(int i = -1; i < 2; i++){
			for(int j = -1; j < 2; j++){
				if(i == 0 && j == 0){
					continue;
				}
				auto neighbor_key = std::make_pair(key.first + i, key.second + j);
				if(map.count(neighbor_key))
					neighbors.push_back(map[neighbor_key]);
			}
		}


	}

}