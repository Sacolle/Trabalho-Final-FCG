#pragma once

#include <unordered_set>
#include <memory>

#include "../entities/entity.hpp"
#include "../entities/camera.hpp"
#include "../renders/shader.hpp"
#include "collision.hpp"

namespace controler{
	class GameLoop{

		GameLoop(std::unique_ptr<entity::Camera> camera,
			std::unique_ptr<CollisionMap> collision_map,
			std::shared_ptr<entity::Player> player,
			std::shared_ptr<render::GPUprogram> gpu_program, 
			std::shared_ptr<render::GPUprogram> wire_renderer);

		~GameLoop();
		//TODO: return a gameover so that it can change screens
		auto next_frame(float delta_time, entity::PressedKeys &keys) -> void;

		auto insert_enemy(std::shared_ptr<entity::Enemy> enemy) -> void;
		auto insert_wall(std::shared_ptr<entity::Wall> wall) -> void;
		auto insert_game_event(std::shared_ptr<entity::GameEvent> game_event) -> void;

		auto remove_enemy(std::shared_ptr<entity::Enemy> enemy) -> void;
		auto remove_wall(std::shared_ptr<entity::Wall> wall) -> void;
		auto remove_game_event(std::shared_ptr<entity::GameEvent> game_event) -> void;

		inline auto set_draw_bbox(bool cond) -> void { draw_bbox = cond; }

	private:
		auto render_frame() -> void;
		auto render_bbox() -> void;
		auto update_player(float delta_time, entity::PressedKeys &keys) -> void;
		auto update_enemies(float delta_time) -> void;
		auto update_camera(float delta_time) -> void;

		std::unique_ptr<entity::Camera> camera;
		std::unique_ptr<CollisionMap> collision_map;
		
		std::shared_ptr<entity::Player> player;

		std::shared_ptr<render::GPUprogram> gpu_program;
		std::shared_ptr<render::GPUprogram> wire_renderer;

		std::unordered_set<std::shared_ptr<entity::Enemy>> enemies;
		std::unordered_set<std::shared_ptr<entity::Wall>> walls;
		std::unordered_set<std::shared_ptr<entity::GameEvent>> game_events;

		int score;
		float time;

		bool draw_bbox;
	};
}