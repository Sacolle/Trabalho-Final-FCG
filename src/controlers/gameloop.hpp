#pragma once
#include <unordered_set>
#include <memory>
#include <iostream>
#include <utility>

#include "../entities/entity.hpp"
#include "../entities/camera.hpp"
#include "../renders/shader.hpp"
#include "collision.hpp"
#include "../utils/matrix.hpp"

namespace controler{
	enum class GameState{
		Loading,
		MainMenu,
		GameOver,
		GameWin,
		Options,
		Playing
	};
	typedef struct CursorState{
		double x, y;
		bool clicked;
	}CursorState;

	class GameLoop{
	public:
		GameLoop(std::unique_ptr<entity::Camera> _camera,
			std::unique_ptr<CollisionMap> _collision_map,
			std::shared_ptr<entity::Player> _player,
			std::shared_ptr<render::GPUprogram> gpu_program,
			std::shared_ptr<render::GPUprogram> wire_renderer,
			entity::PressedKeys *pressed_keys,
			entity::LookAtParameters *look_at_param,
			entity::RotationAngles *rotation_angles,
			CursorState *cursor,
			float *screen_ratio, bool *paused);  

		~GameLoop();
		/*updates to the next gameMoment*/ 
		auto update(float delta_time) -> void; 

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

		auto update_playing(float delta_time) -> void;

		auto update_player(float delta_time, entity::PressedKeys keys) -> std::pair<entity::GameEventTypes, std::shared_ptr<entity::GameEvent>>;
		auto update_enemies(float delta_time) -> entity::GameEventTypes;
		auto update_camera_look_at() -> void;
		auto update_camera_free(float delta_time) -> void;

		/*changes the game state based on the game event*/
		auto handle_event(entity::GameEventTypes game_event_type, std::shared_ptr<entity::GameEvent> game_event) -> void;
		auto is_game_event_event(entity::GameEventTypes game_event_type) -> bool;

		std::unique_ptr<entity::Camera> camera;
		std::unique_ptr<CollisionMap> collision_map;

		std::shared_ptr<entity::Player> player;
		std::unordered_set<std::shared_ptr<entity::Enemy>> enemies;
		std::unordered_set<std::shared_ptr<entity::Wall>> walls;
		std::unordered_set<std::shared_ptr<entity::GameEvent>> game_events;

		//render stuff
		std::shared_ptr<render::GPUprogram> gpu_program;
		std::shared_ptr<render::GPUprogram> wire_renderer;


		//global values changed by glfwcallbacks
		//TODO: include more keys in pressed_keys 
		entity::PressedKeys *pressed_keys;
		entity::LookAtParameters *look_at_param;
		entity::RotationAngles *rotation_angles;
		CursorState *cursor;
		float *screen_ratio;
		bool *paused;

		//FIXME: change to mainMenu when done with the screens
		GameState state = GameState::Playing;

		int score;
		float time;
		bool draw_bbox = true;
	};
}