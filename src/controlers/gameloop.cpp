#include "gameloop.hpp"

namespace controler{
	GameLoop::GameLoop(std::unique_ptr<entity::Camera> _camera,
		std::unique_ptr<CollisionMap> _collision_map,
		std::shared_ptr<entity::Player> _player):
		camera(std::move(_camera)), collision_map(std::move(_collision_map)), player(_player)
	{
		collision_map->insert_mover(player);
	} 

	GameLoop::~GameLoop(){}
	//TODO: return a gameover so that it can change screens 
	/*auto GameLoop::next_frame(entity::PressedKeys &keys, entity::RotationAngles &angles, entity::LookAtParameters &parameters, float delta_time, float screen_ratio, bool paused) -> void {
		if(paused){
			update_camera(keys, angles, delta_time, screen_ratio);
		}
		else {
			update_player(delta_time, keys);
			update_enemies(delta_time);
			update_camera(parameters, screen_ratio);
		}
		
	
		render_frame();
		if(draw_bbox){
			render_bbox(); 
		}
	}
	*/
	auto GameLoop::insert_enemy(std::shared_ptr<entity::Enemy> enemy) -> void {
		enemies.insert(enemy);
		collision_map->insert_mover(enemy);
	}
	auto GameLoop::insert_wall(std::shared_ptr<entity::Wall> wall) -> void {
		walls.insert(wall);
		collision_map->insert_obj(wall);
	}
	auto GameLoop::insert_game_event(std::shared_ptr<entity::GameEvent> game_event) -> void {
		game_events.insert(game_event);
		collision_map->insert_mover(game_event);
	}
	auto GameLoop::remove_enemy(std::shared_ptr<entity::Enemy> enemy) -> void {
		enemies.erase(enemy);
		collision_map->remove_mover(enemy);
	}
	auto GameLoop::remove_wall(std::shared_ptr<entity::Wall> wall) -> void {
		walls.erase(wall);
		collision_map->remove_obj(wall);
	}
	auto GameLoop::remove_game_event(std::shared_ptr<entity::GameEvent> game_event) -> void {
		game_events.erase(game_event);
		collision_map->remove_mover(game_event);
	}

	auto GameLoop::render_frame(std::shared_ptr<render::GPUprogram> program) -> void {
		program->use_prog();
		program->set_mtx("view",camera->get_view_ptr());
		program->set_mtx("projection",camera->get_projection_ptr());

		const auto p_trans = player->get_transform();
		player->draw(p_trans);

		for(auto enemy: enemies){
			enemy->draw(enemy->get_transform());
		}
		for(auto wall: walls){
			wall->draw(wall->get_transform());
		}
		for(auto game_event : game_events){
			game_event->draw(game_event->get_transform());
		}
	} 

	auto GameLoop::render_bbox(std::shared_ptr<render::GPUprogram> program) -> void {
		program->use_prog();
		program->set_mtx("view",camera->get_view_ptr());
		program->set_mtx("projection",camera->get_projection_ptr());

		//TODO: mudar o resto
		const auto pt = player->get_translation() * player->get_bbox_scale();
		player->draw_wire(pt);
		for(auto enemy: enemies){
			const auto et = enemy->get_translation() * enemy->get_bbox_scale();
			enemy->draw_wire(et);
		}
		for(auto wall: walls){
			wall->draw_wire(wall->get_translation());
		}
		for(auto game_event : game_events){
			game_event->draw_wire(game_event->get_translation());
		}
	}

	auto GameLoop::update_player(float delta_time,entity::PressedKeys keys) -> void {
		const bool moved = player->direct_player(keys);
		if(moved){ 
			collision_map->remove_mover(player);

			const auto player_dx = player->get_parcial_direction_x();
			const auto collided_with_dx = collision_map->colide_direction(player,player_dx);
			if(collided_with_dx == nullptr){
				player->translate_direction(player_dx, delta_time);
			}else{
				//std::cout << "it did it X" << std::endl;
				collided_with_dx->collide(player, delta_time);
			}
			const auto player_dz = player->get_parcial_direction_z();
			const auto collided_with_dz = collision_map->colide_direction(player,player_dz);
			if(collided_with_dz == nullptr){
				player->translate_direction(player_dz, delta_time);
			}else{
				//std::cout << "it did it Z" << std::endl;
				collided_with_dz->collide(player, delta_time);
			}

			collision_map->insert_mover(player);
		}
	}

	auto GameLoop::update_enemies(float delta_time) -> void {
		for(auto enemy: enemies){
			collision_map->remove_mover(enemy);
			//point direction towards the player 
			enemy->direct_towards_player(player);

			const auto enemy_dx = enemy->get_parcial_direction_x();
			const auto collided_with_dx = collision_map->colide_direction(enemy,enemy_dx);
			if(collided_with_dx == nullptr){
				enemy->translate_direction(enemy_dx, delta_time);
			}else{
				collided_with_dx->collide(enemy, delta_time);
			}

			const auto enemy_dz = enemy->get_parcial_direction_z();
			const auto collided_with_dz = collision_map->colide_direction(enemy,enemy_dz);
			if(collided_with_dz == nullptr){
				enemy->translate_direction(enemy_dz, delta_time);
			}else{
				collided_with_dz->collide(enemy, delta_time);
			}

			collision_map->insert_mover(enemy);
		}
	}
	auto GameLoop::update_camera(entity::LookAtParameters &parameters, float screen_ratio) -> void {
		camera->update_position(parameters);
		camera->update_aspect_ratio(screen_ratio);
	}
	auto GameLoop::update_camera(entity::PressedKeys &keys, entity::RotationAngles &angles, float delta_time, float screen_ratio) -> void {
		camera->update_position(keys, delta_time);
		camera->update_direction(angles, delta_time);
		camera->update_aspect_ratio(screen_ratio);
	}

}