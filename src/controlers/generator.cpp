#include "generator.hpp"

#include <cstdlib>
#include <iostream>
#include <unordered_set>
#include <algorithm>

namespace controler{
	Generator::Generator(
		std::shared_ptr<render::GPUprogram> phong_phong,
		std::shared_ptr<render::GPUprogram> phong_diffuse,
		std::shared_ptr<render::GPUprogram> gouraud_phong,
		std::shared_ptr<render::GPUprogram> gouraud_diffuse,
		std::shared_ptr<render::GPUprogram> wire_renderer,
		std::shared_ptr<render::WireMesh> cube_wire_mesh,
		std::shared_ptr<render::WireMesh> cylinder_wire_mesh,
		int size, float tile_size
	):phong_phong(phong_phong), phong_diffuse(phong_diffuse),
	gouraud_phong(gouraud_phong), gouraud_diffuse(gouraud_diffuse), wire_renderer(wire_renderer),
	cube_wire_mesh(cube_wire_mesh), cylinder_wire_mesh(cylinder_wire_mesh),
	map_size(size), tile_size(tile_size), wave_map(WaveFuncMap(map_size, 2)){
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
						phong_diffuse,
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
							phong_phong,
							meshes.at(static_cast<int>(MeshIds::HOUSE))
						)
					);
					wall->set_wire_mesh(cube_wire_mesh);
					wall->set_wire_renderer(wire_renderer);

					wall->set_scale(0.05f * tile_size , 0.08f * tile_size , 0.05f * tile_size);
					wall->set_base_translate(2.0f,-4.0f,0.0f);

					wall->set_bbox_size(0.75f * tile_size, 1.0f, 0.45f * tile_size);

					result.walls.push_back(wall);
				}
				//add endpoint
				if(tile_val == 'C'){
					//std::cout << "ADD CAR" << std::endl;
					std::shared_ptr<entity::GameEvent> car(
						new entity::GameEvent(
							glm::vec4(x_pos, 0.0f, z_pos, 1.0f),
							gouraud_phong,
							meshes.at(static_cast<int>(MeshIds::CAR)),
							entity::GameEventTypes::EndPoint
						)
					);
					car->set_wire_mesh(cube_wire_mesh);
					car->set_wire_renderer(wire_renderer);
					car->set_angles(0,1.5707f,0);
					car->set_base_translate(-0.03f,0.05f,0.0f);
					car->set_scale(10.0f,10.0f,10.0f);
					car->set_bbox_size(7.0f,1.0f,3.0f);

					result.game_events.push_back(car);

					ocupied_spaces.insert(idx);
				}
				if(tile_val == '-' || tile_val == '+' || tile_val == '|'){
					//random chance to spawn apoint in a road
					if((rand() % 5) == 0){
						//std::cout << "ADD POINT" << std::endl;
						std::shared_ptr<entity::GameEvent> point(
							new entity::GameEvent(
								glm::vec4(x_pos, 0.0f, z_pos, 1.0f),
								phong_phong,
								meshes.at(static_cast<int>(MeshIds::POINT)),
								entity::GameEventTypes::Point
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
			[&](int idx){return ocupied_spaces.count(idx) == 0;}
		);
		vacant_tile = valid_tiles;

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

		std::shared_ptr<entity::Enemy> enemy(new entity::Enemy(pos, phong_phong, meshes[type]));

		enemy->set_wire_mesh(cylinder_wire_mesh);
		enemy->set_wire_renderer(wire_renderer);

		enemy->set_scale(0.2f,0.2f,0.2f);
		enemy->set_base_direction(glm::vec4(0.0f,0.0f,1.0f,0.0f));
		enemy->set_bbox_type(entity::BBoxType::Cylinder);
		enemy->set_bbox_size(1.0f,2.0f,1.0f);

		enemy->set_speed(0.05f);

		return enemy;
	}
}