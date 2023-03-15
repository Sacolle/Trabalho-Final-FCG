#include "gameloop.hpp"
namespace controler{
	GameLoop::GameLoop(std::unique_ptr<entity::Camera> _camera,
		std::unique_ptr<CollisionMap> _collision_map,
		std::shared_ptr<entity::Player> _player,
		std::shared_ptr<render::GPUprogram> gpu_program,
		std::shared_ptr<render::GPUprogram> wire_renderer,
		entity::PressedKeys *pressed_keys,
		entity::LookAtParameters *look_at_param,
		entity::RotationAngles *rotation_angles,
		CursorState *cursor,
		float *screen_ratio, bool *paused):
		camera(std::move(_camera)), collision_map(std::move(_collision_map)), player(_player),
		gpu_program(gpu_program), wire_renderer(wire_renderer),
		pressed_keys(pressed_keys), look_at_param(look_at_param),
		rotation_angles(rotation_angles), cursor(cursor),
		screen_ratio(screen_ratio), paused(paused)
	{
		collision_map->insert_mover(player);
	} 


	GameLoop::~GameLoop(){}
	auto GameLoop::update(float delta_time) -> void {
		switch (state){
		case GameState::MainMenu :
			/* render the menu screen */
			break;
		case GameState::Options :
			/* render the options screen */
			break;
		case GameState::Loading :
			/* render a loading screen while assets load */
			break;
		case GameState::GameOver :
			/* render the gameOver screen */
			std::cout << "Você perdeu" << std::endl;
			exit(0);
			break;
		case GameState::GameWin :
			/* render the gameOver screen */
			std::cout << "Você ganhou" << std::endl;
			exit(0);
			break;
		case GameState::Playing :
			update_playing(delta_time);
			break;
		default:
			std::cout << "oops" << std::endl;
			break;
		}
	}

	auto GameLoop::update_playing(float delta_time) -> void {
		if(*paused){
			update_camera_free(delta_time);
		}
		else {
			update_camera_look_at();
			const auto event_player = update_player(delta_time,*pressed_keys);
			handle_event(event_player.first, event_player.second);
			const auto event_enemies = update_enemies(delta_time);
			handle_event(event_enemies,nullptr);
		}
		render_frame();
		if(draw_bbox){
			render_bbox(); 
		}
	}

	auto GameLoop::handle_event(entity::GameEventTypes game_event_type, std::shared_ptr<entity::GameEvent> game_event) -> void {
		switch (game_event_type){
		case entity::GameEventTypes::Point :
			remove_game_event(game_event);
			score++;
			std::cout << score << std::endl;
			break;
		case entity::GameEventTypes::EndPoint :
			state = GameState::GameWin;
			break;
		case entity::GameEventTypes::GameOver :
			state = GameState::GameOver;
			break;
		default:
			break;
		}
	}
	auto GameLoop::is_game_event_event(entity::GameEventTypes game_event_type) -> bool {
		switch (game_event_type) {
		case entity::GameEventTypes::Point:
			return true;
			break;
		case entity::GameEventTypes::EndPoint:
			return true;
			break;
		default:
			return false;
			break;
		}
	}
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

	auto GameLoop::render_frame() -> void {
		gpu_program->use_prog();
		gpu_program->set_mtx("view",camera->get_view_ptr());
		gpu_program->set_mtx("projection",camera->get_projection_ptr());

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

	auto GameLoop::render_bbox() -> void {
		wire_renderer->use_prog();
		wire_renderer->set_mtx("view",camera->get_view_ptr());
		wire_renderer->set_mtx("projection",camera->get_projection_ptr());

		//TODO: mudar o resto
		const auto pt = player->get_translation() * player->get_bbox_scale();
		player->draw_wire(pt);
		for(auto enemy: enemies){
			const auto et = enemy->get_translation() * enemy->get_bbox_scale();
			enemy->draw_wire(et);
		}
		for(auto wall: walls){
			const auto wt = wall->get_translation() * wall->get_bbox_scale();
			wall->draw_wire(wt);
		}
		for(auto game_event : game_events){
			const auto gt = game_event->get_translation() * game_event->get_bbox_scale();
			game_event->draw_wire(gt);
		}
	}

	auto GameLoop::update_player(float delta_time, entity::PressedKeys keys) -> std::pair<entity::GameEventTypes, std::shared_ptr<entity::GameEvent>> {
		const bool moved = player->direct_player(keys);
		if(moved){ 
			collision_map->remove_mover(player);
			const auto player_dx = player->get_parcial_direction_x();
			const auto collided_with_dx = collision_map->colide_direction(player,player_dx);
			if(collided_with_dx == nullptr){
				player->translate_direction(player_dx, delta_time);
			}else{
				const auto resulting_event = collided_with_dx->collide(player, delta_time);
				if(is_game_event_event(resulting_event)){
					return std::make_pair(resulting_event, std::dynamic_pointer_cast<entity::GameEvent>(collided_with_dx));
				}else if(resulting_event == entity::GameEventTypes::GameOver){
					return std::make_pair(entity::GameEventTypes::GameOver, nullptr);
				} 
			}
			const auto player_dz = player->get_parcial_direction_z();
			const auto collided_with_dz = collision_map->colide_direction(player,player_dz);
			if(collided_with_dz == nullptr){
				player->translate_direction(player_dz, delta_time);
			}else{
				const auto resulting_event = collided_with_dz->collide(player, delta_time);
				if(is_game_event_event(resulting_event)){
					return std::make_pair(resulting_event, std::dynamic_pointer_cast<entity::GameEvent>(collided_with_dz));
				}else if(resulting_event == entity::GameEventTypes::GameOver){
					return std::make_pair(entity::GameEventTypes::GameOver, nullptr);
				} 
			}
			collision_map->insert_mover(player);
		}
		return std::make_pair(entity::GameEventTypes::None, nullptr);
	}

	auto GameLoop::update_enemies(float delta_time) -> entity::GameEventTypes {
		for(auto enemy: enemies){
			collision_map->remove_mover(enemy);
			//point direction towards the player 
			enemy->direct_towards_player(player);
			const auto enemy_dx = enemy->get_parcial_direction_x();
			const auto collided_with_dx = collision_map->colide_direction(enemy,enemy_dx);
			if(collided_with_dx == nullptr){
				enemy->translate_direction(enemy_dx, delta_time);
			}else{
				const auto resulting_state = collided_with_dx->collide(enemy, delta_time);
				if(is_game_event_event(resulting_state)){
					enemy->translate_direction(enemy_dx, delta_time);
				}else if(resulting_state == entity::GameEventTypes::GameOver){
					return entity::GameEventTypes::GameOver;
				} 
			}

			const auto enemy_dz = enemy->get_parcial_direction_z();
			const auto collided_with_dz = collision_map->colide_direction(enemy,enemy_dz);
			if(collided_with_dz == nullptr){
				enemy->translate_direction(enemy_dz, delta_time);
			}else{
				const auto resulting_state = collided_with_dz->collide(enemy, delta_time);
				if(is_game_event_event(resulting_state)){
					//enemies goes rightthrow gameEvents
					enemy->translate_direction(enemy_dz, delta_time);
				}else if(resulting_state == entity::GameEventTypes::GameOver){
					return entity::GameEventTypes::GameOver;
				} 
			}

			collision_map->insert_mover(enemy);
		}
		return entity::GameEventTypes::None;
	}
	auto GameLoop::update_camera_look_at() -> void {
		camera->update_position(*look_at_param, player->get_cords());
		camera->update_aspect_ratio(*screen_ratio);
	}
	auto GameLoop::update_camera_free(float delta_time) -> void {
		camera->update_position(*pressed_keys, delta_time);
		camera->update_direction(*rotation_angles, delta_time);
		camera->update_aspect_ratio(*screen_ratio);
	}
}