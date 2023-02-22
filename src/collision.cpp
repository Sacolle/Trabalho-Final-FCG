#include "collision.hpp"

#include <iostream>

namespace utils{
	SpacialHash::SpacialHash(float cell_x, float cell_y):
		cell_x(cell_x), cell_y(cell_y){}

	auto SpacialHash::get_quadrant(std::pair<int,int> key) -> std::vector<std::forward_list<Entt>*>{
		std::vector<std::forward_list<Entt>*> out;
		for(int i = -1; i < 2; i++){
			for(int j = -1; j < 2; j++){
				auto local = std::make_pair(key.first + i, key.second + j);
				auto list = get_cell(local);
				if(list != nullptr){
					out.push_back(list);
				}
			}
		}
		return out;
	}
	/*
	//TODO: get_cell can return a nullprt
	auto SpacialHash::get_quadrant(std::pair<int,int> key, int mask) -> NeighborIter  {
		std::vector<std::forward_list<Entt>*> out;
		for(int i = -1; i < 2; i++){
			for(int j = -1; j < 2; j++){
				int cell_bit = 1 << ((3*(i + 1) + j + 1));
				if(mask & cell_bit == cell_bit){
					auto local = std::make_pair(key.first + i, key.second + j);
					out.push_back(get_cell(local));
				}
			}
		}
		return NeighborIter(std::move(out));
	}*/
	/*
	//get_neighbors
	NeighborIter::NeighborIter(std::vector<std::forward_list<std::shared_ptr<entity::Entity>>*> l): lists(l){
		auto ref = lists.back();
		curr = ref->begin();
		end  = ref->end();
	}
	//acess the last list on the vector
	//iterate overt it one step, returning the value
	//when the list is empty, pop it from the vec
	//repete until list is empty 
	auto NeighborIter::next(std::shared_ptr<entity::Entity> *val) -> bool{
		//invalidades new calls for the iterator
		//can be removed for performance, but then when an empty iter calls next, then it goes bad
		if(lists.empty()){
			return false;
		}
		if(curr == end){
			lists.pop_back();
			if(lists.empty()){
				return false;
			}
			auto next_list = lists.back();
			curr = next_list->begin();
			end  = next_list->end();
		}
		*val = *curr;
		curr++;
		return true;
	}*/

	CollisionMap::CollisionMap(float max_width, float max_depth, float mover_cell_grain, float obj_cell_grain):
	max_width(max_width),max_depth(max_depth),mover_cell_grain(mover_cell_grain),obj_cell_grain(obj_cell_grain),
		mover_map(SpacialHash(max_width/mover_cell_grain, max_depth/mover_cell_grain)),
		obj_map(SpacialHash(max_width/obj_cell_grain, max_depth/obj_cell_grain)){}

	auto CollisionMap::insert_obj(Entt obj) -> int {
		auto c_key = obj_map.make_key(obj);
		obj_map.insert(c_key, obj);
		return 1;
	}
	auto CollisionMap::remove_obj(Entt obj) -> int {
		auto key = obj_map.make_key(obj);
		return mover_map.remove(key,obj);
	}
	//for the movable map
	auto CollisionMap::insert_mover(Entt entity) -> int {
		auto key = mover_map.make_key(entity);
		mover_map.insert(key,entity);
		return 1;
	}
	auto CollisionMap::remove_mover(Entt entity) -> int {
		auto key = mover_map.make_key(entity);
		return mover_map.remove(key,entity);
	}
	auto CollisionMap::colide_foward(Entt entity) -> bool {
		//check collision with enemies
		//int i = 0;
		//std::cout << "amount of neighbors: " << mover_neighbors.size() << std::endl;
		auto entity_map_key = mover_map.make_key(entity);
		auto mover_neighbors = mover_map.get_quadrant(entity_map_key);
		for(auto prox: mover_neighbors){
			//std::cout << "i'm " << i++ << std::endl;
			for(auto it = prox->begin(); it != prox->end();++it){
				if(entity->foward_will_collide(*it)){
					return true;
				}
			}
		}
		/*
		auto obj_map_key = obj_map.make_key(entity);
		auto obj_neighbors = obj_map.get_quadrant(obj_map_key);
		for(auto prox: obj_neighbors){
			if(prox == nullptr){
				continue;
			}
			for(auto it = prox->begin(); it != prox->end();++it){
				if(entity->foward_will_collide(*it)){
					return true;
				}
			}
		}*/
		return false;
	}
	/*
	auto CollisionMap::colide_direction(Entt entity, glm::vec4 direction) -> bool {

	}
	auto CollisionMap::make_path(Entt entity, Entt player) -> Path {

	}*/
}