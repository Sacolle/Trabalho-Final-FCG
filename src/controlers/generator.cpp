#include "generator.hpp"

#include <cstdlib>
#include <iostream>
#include <unordered_set>
#include <algorithm>

namespace controler{
	Generator::Generator(
		std::shared_ptr<render::GPUprogram> gpu_program,
		std::shared_ptr<render::GPUprogram> wire_renderer,
		std::shared_ptr<render::WireMesh> cube_wire_mesh,
		std::shared_ptr<render::WireMesh> cylinder_wire_mesh,
		int size, float tile_size
	):gpu_program(gpu_program), wire_renderer(wire_renderer),
	cube_wire_mesh(cube_wire_mesh), cylinder_wire_mesh(cylinder_wire_mesh),
	map_size(size), tile_size(tile_size), wave_map(WaveFuncMap(map_size, 1)){
	}
	Generator::~Generator(){}

	auto Generator::generate_map_elements(int end_points) -> struct MapElements {
		MapElements result;

		generate_char_map(end_points);

		std::unordered_set<int> ocupied_spaces;

		//TODO: set an ofset so that the world center is at 0,0 ?
		//tiles, walls & gameEvents
		for(int z = 0; z < map_size; z++){
			for(int x = 0; x < map_size; x++){
				const int idx = x + z*map_size;
				const char tile_val = char_map.at(idx);

				const float x_pos = x * (2 * tile_size) + (tile_size/2);
				const float z_pos = z * (2 * tile_size) + (tile_size/2);

				//add tile
				//std::cout << "ADD TILE " << tile_val << std::endl;
				std::shared_ptr<entity::Entity> tile(
					new entity::Entity(
						glm::vec4(x_pos,-1.0f,z_pos,1.0f),
						gpu_program,
						tile_meshes.at(tile_val)
					)
				);
				tile->set_scale(tile_size, 1, tile_size);
				result.tiles.push_back(tile);

				//add wall
				if(tile_val == '#'){

					//std::cout << "ADD HOUSE" << std::endl;
					//TODO: vary the size of the houses
					std::shared_ptr<entity::Wall> wall(
						new entity::Wall(
							glm::vec4(x_pos, 0.0f, z_pos, 1.0f),
							gpu_program,
							meshes.at(static_cast<int>(MeshIds::HOUSE))
						)
					);
					wall->set_wire_mesh(cube_wire_mesh);
					wall->set_wire_renderer(wire_renderer);
					result.walls.push_back(wall);
				}
				//add endpoint
				if(tile_val == 'C'){
					//std::cout << "ADD CAR" << std::endl;
					std::shared_ptr<entity::GameEvent> car(
						new entity::GameEvent(
							entity::GameEventTypes::EndPoint,
							glm::vec4(x_pos, 0.0f, z_pos, 1.0f),
							gpu_program,
							meshes.at(static_cast<int>(MeshIds::CAR))
						)
					);
					car->set_wire_mesh(cube_wire_mesh);
					car->set_wire_renderer(wire_renderer);

					car->set_scale(2.0f,1.0f,1.0f);
					car->set_bbox_size(2.0f,1.0f,1.0f);

					result.game_events.push_back(car);

					ocupied_spaces.insert(idx);
				}
				if(tile_val == '-' || tile_val == '+' || tile_val == '|'){
					//random chance to spawn apoint in a road
					if((rand() % 5) == 0){
						//std::cout << "ADD POINT" << std::endl;
						std::shared_ptr<entity::GameEvent> point(
							new entity::GameEvent(
								entity::GameEventTypes::Point,
								glm::vec4(x_pos, 0.0f, z_pos, 1.0f),
								gpu_program,
								meshes.at(static_cast<int>(MeshIds::POINT))
							)
						);
						point->set_wire_mesh(cube_wire_mesh);
						point->set_wire_renderer(wire_renderer);

						point->set_scale(0.5f,0.5f,0.5f);
						point->set_bbox_size(0.5f,0.5f,0.5f);

						result.game_events.push_back(point);

						ocupied_spaces.insert(idx);
					}
				}
			}
		}

		std::vector<int> valid_tiles;		
		std::copy_if(
			vacant_tile.begin(),
			vacant_tile.end(),
			std::back_inserter(valid_tiles),
			[&](int idx){return ocupied_spaces.count(idx) == 1;}
		);
		return result;
	}
	auto Generator::generate_char_map(int end_points) -> void {
		wave_map.set_number_end_points(end_points);
		char_map = wave_map.generate();
		generate_vacant_tiles();
	}
	auto Generator::generate_vacant_tiles() -> void {
		vacant_tile.clear();
		const int size = char_map.size();
		for(int i = 0; i < size; i++){
			if(char_map[i] == ' ' || char_map[i] == '|' || char_map[i] == '-' || char_map[i] == '+'){
				vacant_tile.push_back(i);
			}
		}
	}
	auto Generator::get_vacant_position() -> glm::vec4 {
		const int rand_pos = rand() % vacant_tile.size();
		const int tile_idx = vacant_tile.at(rand_pos);
		const int x = tile_idx % map_size;
		const int z = (tile_idx - x) / map_size;

		return glm::vec4(x * (2 * tile_size) + (tile_size/2), 0.0f, z * (2 * tile_size) + (tile_size/2), 1.0f);
	}
	auto Generator::generate_enemy(int type) -> std::shared_ptr<entity::Enemy> {
		auto pos = get_vacant_position();

		std::shared_ptr<entity::Enemy> enemy(new entity::Enemy(pos, gpu_program, meshes[type]));

		enemy->set_wire_mesh(cylinder_wire_mesh);
		enemy->set_wire_renderer(wire_renderer);

		enemy->set_bbox_size(2.0f,1.0f,2.0f);
		enemy->set_bbox_type(entity::BBoxType::Cylinder);

		return enemy;
	}
}