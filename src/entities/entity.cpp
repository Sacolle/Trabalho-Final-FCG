
#include "entity.hpp"

namespace entity{
	//calculate the normal of the colision and go a little back
	auto take_knock_back(Entity* me, std::shared_ptr<Entity> other, float delta_time) -> void {
		const auto collision_normal = me->get_cords() - other->get_cords();
		me->translate_direction(collision_normal/mtx::norm(collision_normal), delta_time);
	}

	/*
	 * Enemy implementation 
	*/
	//if player is close goes in it's direction
	//else move acording to Path
	//set direction to player  
	auto Enemy::direct_towards_player(std::shared_ptr<Player> player, float delta_time) -> void {
		//if(distan)
		const auto player_dir = player->get_cords() - get_cords();
		const auto normalized = player_dir/mtx::norm(player_dir);
		//if player_dir is small
		//else
		//move acording to path
	}

	auto Enemy::set_damage(int amount) -> void {
		damage = amount;
	}
	auto Enemy::get_damage() -> int {
		return damage;
	}
	//specific collisions for each entity
	auto Enemy::collide(std::shared_ptr<Player> player, float delta_time) -> GameEventTypes {
		take_knock_back(this, player, delta_time);
		player->take_damage(damage);
		if(player->get_life_points() < 0){
			return GameEventTypes::GameOver;
		}
		return GameEventTypes::None;
	}

	auto Enemy::collide(std::shared_ptr<Wall> wall, float delta_time)  -> GameEventTypes {
		take_knock_back(this, wall, delta_time);
		return GameEventTypes::None;
	}
	/*
	 * Player implementation 
	*/

	auto Player::direct_player(PressedKeys &keys) -> bool {
		const float angle = player_angle_from_keys(keys);
		if(angle == -1)
			return false;
		set_y_angle(angle);
		return true;
	}
	auto Player::player_angle_from_keys(PressedKeys &keys) -> float {
		const float pi = 3.141592f;
		float components = 0;
		float angle = 0;
		if(keys.w){
			angle += pi/2;
			components++;
		}
		if(keys.a){
			angle += pi;
			components++;
		}
		if(keys.s){
			angle += pi + pi/2;
			components++;
		}
		if(keys.d){
			if(angle >= pi){
				angle += pi + pi;
			}else{
				angle += 0;
			}
			components++;
		}
		if(components == 0) return -1;
		return angle/components;
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
		take_knock_back(this, enemy, delta_time);
		take_damage(enemy->get_damage());
		if(get_life_points() < 0){
			return GameEventTypes::GameOver;
		}
		return GameEventTypes::None;
	}
	auto Player::collide(std::shared_ptr<Wall> wall, float delta_time) -> GameEventTypes {
		take_knock_back(this, wall, delta_time);
		return GameEventTypes::None;
	}
	auto Player::collide(std::shared_ptr<GameEvent> game_event, float delta_time) -> GameEventTypes {
		return game_event->get_type();
	}
}


