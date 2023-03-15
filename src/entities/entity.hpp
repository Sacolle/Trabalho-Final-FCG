#pragma once
#include <memory>
#include "../renders/renderable.hpp"
#include "../renders/shader.hpp"
#include "../renders/mesh.hpp"
#include "geometry.hpp"
#include "../utils/matrix.hpp"
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
	/*
	* Okay, meio entendimento de single dispatch tava meio errado
	* Dado q o player colidiu co algo no collision map, su vou chamar a função collide dessa entidade desconhecida
	* Ou seja, 
	*	- a parede deve ter funções que empurram o player e o inimigo
	*   - o inimigo deve ter funções que empurra o player (e outros inimigos se pá)
	*   - o game event deve ter funções q retornam o seu tipo quando colide com o player
	*   - o player deve ter funções que empurram o inimigo 
	*/
	//base do polimorfismo
	class Entity : public Geometry, public render::Renderable {
		//used both for single dispatch and to unite the Geometry and Renderable into one class
		public:
			Entity(glm::vec4 cords, 
				std::shared_ptr<render::GPUprogram> gpu_program,
				std::shared_ptr<render::Mesh> mesh): Geometry(cords), render::Renderable(gpu_program, mesh){} 
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
			Enemy(glm::vec4 cords, 
				std::shared_ptr<render::GPUprogram> gpu_program,
				std::shared_ptr<render::Mesh> mesh): Entity(cords, gpu_program, mesh){} 
			Enemy(){}
			virtual ~Enemy(){}
			//if player is close goes in it's direction
			//else move acording to Path
			auto direct_towards_player(std::shared_ptr<Player> player) -> void;
			auto set_damage(int amount) -> void;
			auto get_damage() -> int;
			//specific collisions for each entity
			virtual auto collide(std::shared_ptr<Player> player, float delta_time) -> GameEventTypes override;
			//virtual auto collide(std::shared_ptr<Wall> wall, float delta_time)  -> GameEventTypes override;
		private: 
			//Path path;
			int damage = 1;
	};
	typedef struct PressedKeys{
		bool w, a, s, d;
	} PressedKeys;
	class Player : public Entity {
		public:
			
			Player(glm::vec4 cords, 
				std::shared_ptr<render::GPUprogram> gpu_program,
				std::shared_ptr<render::Mesh> mesh): Entity(cords, gpu_program, mesh){} 
			Player(){}
			virtual ~Player(){}
			auto direct_player(PressedKeys &keys) -> bool;
			auto take_damage(int amount) -> void;
			auto set_life_points(int amount) -> void;
			auto get_life_points() -> int;
			
			//specific collisions for each entity
			virtual auto collide(std::shared_ptr<Enemy> enemy, float delta_time) -> GameEventTypes override;
			//virtual auto collide(std::shared_ptr<Wall> wall, float delta_time) -> GameEventTypes override;
			//virtual auto collide(std::shared_ptr<GameEvent> game_event, float delta_time) -> GameEventTypes override;
		private: 
			auto player_angle_from_keys(PressedKeys &keys) -> float;
			int life_points = 3;
	};
	//this is needed so that the single dispatch works
	class Wall : public Entity {
		public:
			Wall(glm::vec4 cords, 
				std::shared_ptr<render::GPUprogram> gpu_program,
				std::shared_ptr<render::Mesh> mesh): Entity(cords, gpu_program, mesh){} 
			Wall(){}
			virtual ~Wall(){}

			virtual auto collide(std::shared_ptr<Enemy> enemy, float delta_time) -> GameEventTypes override;
			virtual auto collide(std::shared_ptr<Player> player, float delta_time) -> GameEventTypes override;
	};

	class GameEvent : public Entity {
		public:
			GameEvent(GameEventTypes type,
				glm::vec4 cords, 
				std::shared_ptr<render::GPUprogram> gpu_program,
				std::shared_ptr<render::Mesh> mesh): type(type), Entity(cords, gpu_program, mesh){} 
			GameEvent(){}
			virtual ~GameEvent(){}

			inline auto get_type() -> GameEventTypes { return type; }
			//inline auto set_type(GameEventTypes new_type) -> void { type = new_type; }

			virtual auto collide(std::shared_ptr<Player> player, float delta_time) -> GameEventTypes override;
			virtual auto collide(std::shared_ptr<Enemy> enemy, float delta_time) -> GameEventTypes override;
		private:
			GameEventTypes type;
	};
}