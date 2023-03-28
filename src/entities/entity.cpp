#include "entity.hpp"

#include <cmath>
#include <iostream>

namespace entity{
	//calculate the normal of the colision and go a little back
	auto cause_knock_back(Entity* me, std::shared_ptr<Entity> other, float delta_time) -> void {
		const auto collision_normal = other->get_cords() - me->get_cords();
		other->translate_direction(collision_normal/mtx::norm(collision_normal), delta_time);
	}
	/*
	 * Enemy implementation 
	*/
	//if player is close goes in it's direction
	//else move acording to Path
	//set direction to player  
	auto Enemy::direct_towards_player(std::shared_ptr<Player> player) -> void {
		const float pi = 3.141592f;

		const auto player_dir = player->get_cords() - get_cords();
		const auto base_dir = get_base_direction();

		const auto angle = atan2f(base_dir.z, base_dir.x) - atan2f(player_dir.z, player_dir.x);
		set_y_angle(angle < 0 ? angle + 2*pi : angle);
	}

	auto Enemy::set_damage(int amount) -> void {
		damage = amount;
	}
	auto Enemy::get_damage() -> int {
		return damage;
	}
	//specific collisions for each entity
	auto Enemy::collide(std::shared_ptr<Player> player, float delta_time) -> GameEventTypes {
		cause_knock_back(this, player, delta_time);
		player->take_damage(damage);
		if(player->get_life_points() < 0){
			return GameEventTypes::GameOver;
		}
		return GameEventTypes::None;
	}
	/* 
	auto Enemy::collide(std::shared_ptr<Wall> wall, float delta_time)  -> GameEventTypes {
		take_knock_back(this, wall, delta_time);
		return GameEventTypes::None;
	}*/
	/*
	 * Player implementation 
	*/
	auto Player::direct_player(PressedKeys &keys, glm::vec4 dir, glm::vec4 up_vec) -> bool {
		const float angle = player_angle_from_keys(keys, dir, up_vec);
		if(angle == -1)
			return false;
		set_y_angle(angle);
		return true;
	}
	auto Player::player_angle_from_keys(PressedKeys &keys, glm::vec4 dir, glm::vec4 up_vec) -> float {
		glm::vec4 result(0.0f,0.0f,0.0f,0.0f);
		dir.y = 0;
		glm::vec4 frente  = dir;
		glm::vec4 direita = dir * mtx::rotate_rodriguez(PI/2,up_vec);

		if(keys.w){
			result += frente;
		}
		if(keys.a){
			result -= direita;
		}
		if(keys.s){
			result -= frente;
		}
		if(keys.d){
			result += direita;
		}
		if(result == glm::vec4(0.0f,0.0f,0.0f,0.0f)) return -1;

		const auto base_dir = get_base_direction();
		//como gerar o angulo baseado nos vetores de 0 a 2pi
		//https://math.stackexchange.com/a/2234559
		const auto angle = atan2f(base_dir.z, base_dir.x) - atan2f(result.z, result.x);

		return angle < 0 ? angle + 2*PI : angle;
	}
	auto Player::take_damage(int amount) -> void {
		life_points -= amount;
	}
	auto Player::set_life_points(int amount) -> void {
		life_points = amount;
	}
	auto Player::get_life_points() -> int {
		return life_points;
	}
	//specific collisions for each entity
	auto Player::collide(std::shared_ptr<Enemy> enemy, float delta_time) -> GameEventTypes {
		cause_knock_back(this, enemy, delta_time);
		take_damage(enemy->get_damage());
		if(get_life_points() < 0){
			return GameEventTypes::GameOver;
		}
		return GameEventTypes::None;
	}
	/*
	auto Player::collide(std::shared_ptr<Wall> wall, float delta_time) -> GameEventTypes {
		take_knock_back(this, wall, delta_time);
		return GameEventTypes::None;
	}
	auto Player::collide(std::shared_ptr<GameEvent> game_event, float delta_time) -> GameEventTypes {
		return game_event->get_type();
	}*/
	auto Wall::collide(std::shared_ptr<Enemy> enemy, float delta_time) -> GameEventTypes {
		cause_knock_back(this, enemy, delta_time);
		return GameEventTypes::None;
	}
	auto Wall::collide(std::shared_ptr<Player> player, float delta_time) -> GameEventTypes {
		cause_knock_back(this, player, delta_time);
		return GameEventTypes::None;
	}
	auto GameEvent::collide(std::shared_ptr<Player> player, float delta_time) -> GameEventTypes {
		return type;
	}
	auto GameEvent::collide(std::shared_ptr<Enemy> enemy, float delta_time) -> GameEventTypes {
		return type;
	}
}


