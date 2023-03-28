#include "gameloop.hpp"
namespace controler{
	GameLoop::GameLoop(std::unique_ptr<entity::Camera> _camera,
		std::unique_ptr<CollisionMap> _collision_map,
		std::unique_ptr<Generator> _generator,
		std::shared_ptr<entity::Player> _player,
		std::shared_ptr<render::GPUprogram> phong_phong,
		std::shared_ptr<render::GPUprogram> phong_diffuse,
		std::shared_ptr<render::GPUprogram> gouraud_phong,
		std::shared_ptr<render::GPUprogram> gouraud_diffuse,
		std::shared_ptr<render::GPUprogram> wire_renderer,
		std::shared_ptr<render::GPUprogram> menu_renderer,
		entity::PressedKeys *pressed_keys,
		entity::LookAtParameters *look_at_param,
		entity::RotationAngles *rotation_angles,
		CursorState *cursor,
		float *screen_ratio, bool *paused):
		camera(std::move(_camera)), collision_map(std::move(_collision_map)), generator(std::move(_generator)),
		player(_player),
		phong_phong(phong_phong), phong_diffuse(phong_diffuse),
		gouraud_phong(gouraud_phong), gouraud_diffuse(gouraud_diffuse),
		wire_renderer(wire_renderer), menu_renderer(menu_renderer),
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
			update_menu();
			//setup_playing_state();
			//std::cout << "generated map" << std::endl;
			//state = GameState::Playing;
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

	auto GameLoop::setup_playing_state() -> void {
		auto map_elements = generator->generate_map_elements(2);

		for(const auto &tile : map_elements.tiles){
			insert_background(tile);
		}
		for(const auto &wall : map_elements.walls){
			insert_wall(wall);
		}
		for(const auto &ge : map_elements.game_events){
			insert_game_event(ge);
		}
		const auto valid_position = generator->get_vacant_position();
		collision_map->remove_mover(player);
		player->set_cords(valid_position.x, valid_position.y, valid_position.z);
		collision_map->insert_mover(player);
	}
	auto GameLoop::clear_playing_state() -> void {
		score = 0;
		time = 0;
		enemies.clear();
		game_events.clear();
		walls.clear();
		background.clear();
	}

	auto GameLoop::update_menu() -> void {
		
		entity::LookAtParameters menu_view{0,0,1.5};
		camera->update_position(menu_view, glm::vec4(0.0f,0.0f,0.0f,1.0f));
		camera->update_aspect_ratio(*screen_ratio);

		menu_renderer->use_prog();
		menu_renderer->set_mtx("view",camera->get_view_ptr());
		menu_renderer->set_mtx("projection",camera->get_projection_ptr());

		const auto menu_screen = screens.at(GameState::MainMenu);
		menu_screen->draw(menu_screen->get_transform());
		menu_screen->draw_nodes();
		if(cursor->clicked){
			std::cout << "x: " << cursor->x << " y: " << cursor->y << std::endl;
			const auto action = menu_screen->click_action(cursor->x,cursor->y,800,800);
			switch (action){
			case entity::MenuOptions::Play:
				std::cout << "Play" << std::endl;
				state = GameState::Playing;
				setup_playing_state();
				break;
			case entity::MenuOptions::Exit:
				std::cout << "Thanks for Playing" << std::endl;
				exit(0);
				break;
			case entity::MenuOptions::Retry:
				state = GameState::Playing;
				clear_playing_state();
				setup_playing_state();
				break;
			default:
				std::cout << "None" << std::endl;
				break;
			}
		}
	}
	auto GameLoop::update_playing(float delta_time) -> void {
		if(*paused){
			update_camera_free(delta_time);
		}
		else {
			time += delta_time;
			if(static_cast<int>(time) % spawn_rate == 0){
				std::cout << "spawn enemy" << std::endl;
				auto new_enemy = generator->generate_enemy(static_cast<int>(MeshIds::ENEMY));
				insert_enemy(new_enemy);
			}

			update_camera_look_at();
			//std::cout << "update player" << std::endl;
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
			std::cout << ++score << std::endl;
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
	auto GameLoop::insert_background(std::shared_ptr<entity::Entity> bg) -> void {
		background.insert(bg);
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
	auto GameLoop::remove_background(std::shared_ptr<entity::Entity> bg) -> void {
		background.erase(bg);
	}

	auto GameLoop::render_frame() -> void {
		phong_phong->use_prog();
		phong_phong->set_mtx("view",camera->get_view_ptr());
		phong_phong->set_mtx("projection",camera->get_projection_ptr());
		const auto p_trans = player->get_transform();
		player->draw(p_trans);
		gouraud_phong->use_prog();
		gouraud_phong->set_mtx("view",camera->get_view_ptr());
		gouraud_phong->set_mtx("projection",camera->get_projection_ptr());
		for(auto enemy: enemies){
			enemy->draw(enemy->get_transform());
		}
		phong_diffuse->use_prog();
		phong_diffuse->set_mtx("view",camera->get_view_ptr());
		phong_diffuse->set_mtx("projection",camera->get_projection_ptr());
		for(auto wall: walls){
			wall->draw(wall->get_transform());
		}
		for(auto game_event : game_events){
			game_event->draw(game_event->get_transform());
		}
		gouraud_diffuse->use_prog();
		gouraud_diffuse->set_mtx("view",camera->get_view_ptr());
		gouraud_diffuse->set_mtx("projection",camera->get_projection_ptr());
		for(auto bg : background){
			bg->draw(bg->get_transform());
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
		const bool moved = player->direct_player(keys, camera->get_direction(), camera->get_up_vec());
		if(moved){ 
			//std::cout << "player position \n\tx: " << player->get_cords().x << "\n\tz: " << player->get_cords().z << std::endl;
			int n_removed = collision_map->remove_mover(player);
			assert(n_removed == 1);

			const auto player_dx = player->get_parcial_direction_x();
			const auto collided_with_dx = collision_map->colide_direction(player,player_dx);
			if(collided_with_dx == nullptr){
				player->translate_direction(player_dx, delta_time);
			}else{
				const auto resulting_event = collided_with_dx->collide(player, delta_time);
				if(is_game_event_event(resulting_event)){
					std::cout << "event" << std::endl;
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
			//increasse speed based on time
			if(static_cast<int>(time) % speed_increasse_rate == 0){
				const float enemy_speed = enemy->get_speed();
				enemy->set_speed(enemy_speed + speed_increasse);
			}

			int n_removed = collision_map->remove_mover(enemy);
			assert(n_removed == 1);
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