#pragma once

#include <tuple>
#include <unordered_map>
#include <unordered_set>
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
	struct tuple_hash : std::unary_function<std::tuple<int,int,int>, std::size_t>{
		std::size_t operator()(std::tuple<int,int,int> const& e) const {
			std::size_t seed = 0;
			hash_combine(seed,std::get<0>(e));
			hash_combine(seed,std::get<1>(e));
			hash_combine(seed,std::get<2>(e));
			return seed;
		}
	};
	//podia ser facil, mas n~]aoooooooooooooo
	struct tuple_equal_to : std::binary_function<std::tuple<int,int,int>, std::tuple<int,int,int>, bool>
	{
		bool operator()(std::tuple<int,int,int> const& x, std::tuple<int,int,int> const& y) const
		{
			return (std::get<0>(x)==std::get<0>(y) &&
					std::get<1>(x)==std::get<1>(y) &&
					std::get<2>(x)==std::get<2>(y));
		}
	};

	class SpacialHash {
		public:
			//constructor
			SpacialHash(float cell_x, float cell_y, float cell_z);

			//insert the shared_ptrs at every cell the entity ocupies,
			//as cords(x, y) + it's bounding box
			//return the num of cells it ocupies
			auto insert(std::shared_ptr<entity::Entity> elem) -> int;
			
			//remove the shared_ptr at the center,
			//then at every other cell it also ocupies (x, y) + bbox
			//return the num of cells it removed from
			auto remove(std::shared_ptr<entity::Entity> elem) -> int;

			//return a set of all the entities nearby
			auto get_neighbors(std::shared_ptr<entity::Entity> elem) -> std::unordered_set<std::shared_ptr<entity::Entity>>;

		private:
			//have it at minimum the size of the biggest enemy
			float cell_x, cell_y, cell_z;

			std::unordered_map<
				std::tuple<int,int,int>,
				std::unordered_set<std::shared_ptr<entity::Entity>>,
				tuple_hash, tuple_equal_to> map;
	};
	
}