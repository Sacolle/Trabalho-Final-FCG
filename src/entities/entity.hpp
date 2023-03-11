#pragma once

#include <memory>

#include "../renders/renderable.hpp"
#include "geometry.hpp"


namespace entity{
	enum class GameEventTypes{
		EndPoint,
		GameOver,
		Point,
		None
	};

	class Enemy;
	class Player;
	class Wall;
	class GameEvent;

	class Entity : public Geometry, public render::Renderable {
		//used both for single dispatch and to unite the Geometry and Renderable into one class
		public:
			Entity(){}
			virtual ~Entity(){}
			virtual auto collide(std::shared_ptr<Enemy> enemy, float delta_time) -> GameEventTypes {
				return GameEventTypes::None;
			}
			virtual auto collide(std::shared_ptr<Player> player, float delta_time) -> GameEventTypes {
				return GameEventTypes::None;
			}
			virtual auto collide(std::shared_ptr<Wall> wall, float delta_time)  -> GameEventTypes {
				return GameEventTypes::None;
			}
			virtual auto collide(std::shared_ptr<GameEvent> game_event, float delta_time) -> GameEventTypes {
				return GameEventTypes::None;
			}
	};

	class Enemy : public Entity {
		public:
			//Enemy(){}
			//virtual ~Enemy(){}

			//if player is close goes in it's direction
			//else move acording to Path
			auto direct_towards_player(std::shared_ptr<Player> player, float delta_time) -> void;

			auto set_damage(int amount) -> void;
			auto get_damage() -> int;

			//specific collisions for each entity
			virtual auto collide(std::shared_ptr<Player> player, float delta_time) -> GameEventTypes override;
			virtual auto collide(std::shared_ptr<Wall> wall, float delta_time)  -> GameEventTypes override;
		private: 
			//Path path;
			int damage = 1;
	};

	typedef struct PressedKeys{
		bool w, a, s, d;
	} PressedKeys;
	class Player : public Entity {
		public:
			//Player(){}
			//virtual ~Player(){}

			auto direct_player(PressedKeys &keys) -> bool;

			auto take_damage(int amount) -> void;

			auto set_life_points(int amount) -> void;
			auto get_life_points() -> int;
			
			//specific collisions for each entity
			virtual auto collide(std::shared_ptr<Enemy> enemy, float delta_time) -> GameEventTypes override;
			virtual auto collide(std::shared_ptr<Wall> wall, float delta_time) -> GameEventTypes override;
			virtual auto collide(std::shared_ptr<GameEvent> game_event, float delta_time) -> GameEventTypes override;
		private: 
			auto player_angle_from_keys(PressedKeys &keys) -> float;

			int life_points = 3;
	};

	//this is needed so that the single dispatch works
	class Wall : public Entity {};

	class GameEvent : public Entity {
		public:
			inline auto get_type() -> GameEventTypes { return type; }
		private:
			GameEventTypes type;

	};
}
