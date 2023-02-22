#pragma once

#include <utility>
#include <unordered_map>
#include <forward_list>
#include <memory>
#include <cmath>
#include <vector>

#include <glm/vec4.hpp>

#include "entities.hpp"

namespace utils{
	/*****************
		EU ODEIO C++
	******************/
	template <class T>
	inline void hash_combine(std::size_t& seed, const T& v){
		std::hash<T> hasher;
		seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
	}
	//realmente
	struct pair_hash : std::unary_function<std::pair<int,int>, std::size_t>{
		std::size_t operator()(std::pair<int,int> const& e) const {
			std::size_t seed = 0;
			hash_combine(seed,e.first);
			hash_combine(seed,e.second);
			return seed;
		}
	};
	//podia ser facil, mas nãoooooooooooooo
	struct pair_equal_to : std::binary_function<std::pair<int,int>, std::pair<int,int>, bool>
	{
		bool operator()(std::pair<int,int> const& x, std::pair<int,int> const& y) const
		{
			return (x.first == y.first && x.second == y.second);
		}
	};

	//for x y cords
	using Entt = std::shared_ptr<entity::Entity>;
	/*
	class NeighborIter{
		public:
			NeighborIter(std::vector<std::forward_list<Entt>*> l);

			auto next(Entt *val) -> bool;
		private:
			std::vector<std::forward_list<Entt>*> lists;
			std::forward_list<Entt> :: iterator curr;
			std::forward_list<Entt> :: iterator end;
	}; */

	class SpacialHash {
		public:
			SpacialHash(float cell_x, float cell_y);

			inline auto make_key(Entt elem) const -> std::pair<int,int>{
				int center_x = static_cast<int>(floorf(elem->get_x_cords()/cell_x));
				int center_y = static_cast<int>(floorf(elem->get_z_cords()/cell_y));
				return std::make_pair(center_x,center_y);
			}
			inline auto insert(std::pair<int,int> key, Entt elem) -> void {
				map[key].push_front(elem);
			}
			inline auto remove(std::pair<int,int> key, Entt elem) -> int {
				if(!map.count(key)){
					return 0;
				}
				map[key].remove(elem);
				return 1;
			}
			inline auto get_cell(std::pair<int,int> key) -> std::forward_list<Entt>* {
				if(map.count(key) == 0){
					return nullptr;
				}
				return &map[key];
			}
			auto get_quadrant(std::pair<int,int> key) -> std::vector<std::forward_list<Entt>*>;
			//auto get_quadrant(std::pair<int,int> key, int mask) -> NeighborIter;
		private:
			//have it at minimum the size of the biggest enemy
			const float cell_x, cell_y;

			std::unordered_map<
				std::pair<int,int>,
				std::forward_list<Entt>,
				pair_hash, pair_equal_to> map;
	};
	class Path{
		//stuff
		private:
			float weight;
			std::vector<glm::vec4> points;
	};
	/*
	Has the job to handle collisions and generate paths
		CollisionMap col;
		if(!col.colide_foward(entity)) *does the moving stuff*;
	*/
	class CollisionMap{
		public:
			CollisionMap(float max_width, float max_depth, float mover_cell_grain, float obj_cell_grain);
			//for the imovable map
			auto insert_obj(Entt obj) -> int;
			auto remove_obj(Entt obj) -> int;
			//for the movable map
			auto insert_mover(Entt entity) -> int;
			auto remove_mover(Entt entity) -> int;

			auto colide_foward(Entt entity) -> bool;
			auto colide_direction(Entt entity, glm::vec4 direction) -> bool;

			auto make_path(Entt entity, Entt player) -> Path;
		private:
			const float max_width, max_depth;
			const float mover_cell_grain, obj_cell_grain;

			SpacialHash obj_map;
			SpacialHash mover_map;
	};

}