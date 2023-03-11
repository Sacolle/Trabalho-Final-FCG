#include "gameloop.hpp"

namespace controler{
	GameLoop::GameLoop(std::unique_ptr<entity::Camera> camera,
		std::unique_ptr<CollisionMap> collision_map,
		std::shared_ptr<entity::Player> player, 
		std::shared_ptr<render::GPUprogram> gpu_program, 
		std::shared_ptr<render::GPUprogram> wire_renderer):
		camera(std::move(camera)), collision_map(std::move(collision_map)), player(player),
		gpu_program(gpu_program), wire_renderer(wire_renderer){} 

	GameLoop::~GameLoop(){}
	//TODO: return a gameover so that it can change screens 
	auto GameLoop::next_frame(float delta_time, entity::PressedKeys &keys) -> void {
		update_player(delta_time, keys);
		update_enemies(delta_time);
		update_camera(delta_time);
		/*
		camera->update_position(phi,theta,distance);
		camera->update_aspect_ratio(g_ScreenRatio);
		gpu_program->use_prog();
		gpu_program->set_mtx("view",camera->get_view_ptr());
		gpu_program->set_mtx("projection",camera->get_projection_ptr());
		*/  
		render_frame();
		if(draw_bbox){
			render_bbox(); 
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

		player->draw(player->get_transform());
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

		player->draw_wire(player->get_translation());
		for(auto enemy: enemies){
			enemy->draw_wire(enemy->get_translation());
		}
		for(auto wall: walls){
			wall->draw_wire(wall->get_translation());
		}
		for(auto game_event : game_events){
			game_event->draw_wire(game_event->get_translation());
		}
	}
	auto GameLoop::update_player(float delta_time, entity::PressedKeys &keys) -> void {

	}
	auto GameLoop::update_enemies(float delta_time) -> void {

	}
	auto GameLoop::update_camera(float delta_time) -> void {

	}
}