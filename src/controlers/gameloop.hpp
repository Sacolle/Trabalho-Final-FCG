#pragma once

#include <unordered_set>
#include <memory>
#include <iostream>

#include "../entities/entity.hpp"
#include "../entities/camera.hpp"
#include "../renders/shader.hpp"
#include "collision.hpp"
#include "../utils/matrix.hpp"

namespace controler{
	class GameLoop{
	public:
		GameLoop(std::unique_ptr<entity::Camera> _camera,
			std::unique_ptr<CollisionMap> _collision_map,
			std::shared_ptr<entity::Player> _player); 

		~GameLoop();
		//TODO: return a gameover so that it can change screens
		auto render_frame(std::shared_ptr<render::GPUprogram> program) -> void;
		auto render_bbox(std::shared_ptr<render::GPUprogram> program) -> void;
		auto update_player(float delta_time, entity::PressedKeys keys) -> void;
		auto update_enemies(float delta_time) -> void;
		auto update_camera(float phi, float theta, float distance, float delta_time) -> void;

		auto insert_enemy(std::shared_ptr<entity::Enemy> enemy) -> void;
		auto insert_wall(std::shared_ptr<entity::Wall> wall) -> void;
		auto insert_game_event(std::shared_ptr<entity::GameEvent> game_event) -> void;

		auto remove_enemy(std::shared_ptr<entity::Enemy> enemy) -> void;
		auto remove_wall(std::shared_ptr<entity::Wall> wall) -> void;
		auto remove_game_event(std::shared_ptr<entity::GameEvent> game_event) -> void;

		inline auto set_draw_bbox(bool cond) -> void { draw_bbox = cond; }
	private:
		std::unique_ptr<entity::Camera> camera;
		std::unique_ptr<CollisionMap> collision_map;
		
		std::shared_ptr<entity::Player> player;

		std::unordered_set<std::shared_ptr<entity::Enemy>> enemies;
		std::unordered_set<std::shared_ptr<entity::Wall>> walls;
		std::unordered_set<std::shared_ptr<entity::GameEvent>> game_events;

		int score;
		float time;

		bool draw_bbox;
	};
}