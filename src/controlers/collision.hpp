#pragma once

#include <utility>
#include <unordered_map>
#include <forward_list>
#include <memory>
#include <cmath>
#include <vector>

#include <glm/vec4.hpp>

#include "../entities/entity.hpp"

namespace controler{
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
	class SpacialHash {
		public:
			SpacialHash(float cell_x, float cell_y);

			auto make_key(Entt elem) const -> std::pair<int,int>;
			auto insert(std::pair<int,int> key, Entt elem) -> void;
			auto remove(std::pair<int,int> key, Entt elem) -> int;
			auto get_cell(std::pair<int,int> key) -> std::forward_list<Entt>*;
			auto get_quadrant(std::pair<int,int> key) -> std::vector<std::forward_list<Entt>*>;

			auto log() const -> void;
			auto clear() -> void;
			//auto get_quadrant(std::pair<int,int> key, int mask) -> NeighborIter;
		private:
			//have it at minimum the size of the biggest enemy
			const float cell_x, cell_y;

			std::unordered_map<
				std::pair<int,int>,
				std::forward_list<Entt>,
				pair_hash, pair_equal_to> map;
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

			auto clear() -> void;
			//auto colide_foward(Entt entity) -> bool;
			auto colide_direction(Entt entity, const glm::vec4 direction) -> Entt;
		private:
			auto direction_will_collide(Entt target ,Entt geometry, const glm::vec4 &dir) -> Entt;
			auto box_to_box_collision(Entt target ,Entt geometry, const glm::vec4 future_pos) const -> bool;
			auto cilinder_to_box_collision(Entt target ,Entt geometry, const glm::vec4 future_pos) const -> bool;
			auto box_to_cilinder_collision(Entt target ,Entt geometry, const glm::vec4 future_pos) const -> bool;
			auto cilinder_to_cilinder_collision(Entt target ,Entt geometry, const glm::vec4 future_pos) const -> bool;
			
			const float max_width, max_depth;
			const float mover_cell_grain, obj_cell_grain;

			SpacialHash obj_map;
			SpacialHash mover_map;
	};
}