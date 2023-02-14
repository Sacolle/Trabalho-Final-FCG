#pragma once

#include <utility>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>

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
	//podia ser facil, mas n~]aoooooooooooooo
	struct pair_equal_to : std::binary_function<std::pair<int,int>, std::pair<int,int>, bool>
	{
		bool operator()(std::pair<int,int> const& x, std::pair<int,int> const& y) const
		{
			return (x.first == y.first && x.second == y.second);
		}
	};
	//for x y cords
	class SpacialHash {
		public:
			//constructor
			SpacialHash(float cell_x, float cell_y);

			//insert the shared_ptrs at every cell the entity ocupies,
			//as cords(x, y) + it's bounding box
			//return the num of cells it ocupies
			auto insert(std::shared_ptr<entity::Entity> elem) -> int;
			
			//remove the shared_ptr at the center,
			//then at every other cell it also ocupies (x, y) + bbox
			//return the num of cells it removed from
			auto remove(std::shared_ptr<entity::Entity> elem) -> int;

			//return a set of all the entities nearby
			auto get_neighbors(std::shared_ptr<entity::Entity> elem)
				-> std::vector<std::unordered_set<std::shared_ptr<entity::Entity>>&>;

		private:
			//have it at minimum the size of the biggest enemy
			float cell_x, cell_y;

			std::unordered_map<
				std::pair<int,int>,
				std::unordered_set<std::shared_ptr<entity::Entity>>,
				pair_hash, pair_equal_to> map;


			auto get_key(std::shared_ptr<entity::Entity> elem) -> std::pair<int,int>;
	};
	
}