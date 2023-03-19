#pragma once

#include <vector>
#include <unordered_map>
#include <utility>

#include "../renders/mesh.hpp"
#include "../renders/shader.hpp"
#include "../entities/entity.hpp"
#include "gamemap.hpp"

namespace controler{
	struct MapElements{
		std::vector<std::shared_ptr<entity::Entity>> tiles;
		std::vector<std::shared_ptr<entity::Wall>> walls;
		std::vector<std::shared_ptr<entity::GameEvent>> game_events;
	};
	enum class MeshIds{
		ENEMY = 0,
		HOUSE = 1,
		CAR = 2,
		POINT = 3,
	};

	class Generator{
		public:
			Generator(
				std::shared_ptr<render::GPUprogram> gpu_program,
				std::shared_ptr<render::GPUprogram> wire_renderer,
				std::shared_ptr<render::WireMesh> cube_wire_mesh,
				std::shared_ptr<render::WireMesh> cylinder_wire_mesh,
				int size, float tile_size
			);
			~Generator();

			auto generate_map_elements(int end_points) -> struct MapElements;
			auto generate_enemy(int type) -> std::shared_ptr<entity::Enemy>;

			auto get_vacant_position() -> glm::vec4;

			inline auto insert_mesh(int mesh_id, std::shared_ptr<render::Mesh> mesh) -> void {
				meshes[mesh_id] = mesh;
			}
			inline auto insert_tile_mesh(char tile, std::shared_ptr<render::Mesh> mesh) -> void {
				tile_meshes[tile] = mesh;
			}

		private:
			auto generate_vacant_tiles() -> void;
			auto generate_char_map(int end_points) -> void;

			std::unordered_map<int, std::shared_ptr<render::Mesh>> meshes;

			std::shared_ptr<render::GPUprogram> gpu_program;
			std::shared_ptr<render::GPUprogram> wire_renderer;

			std::shared_ptr<render::WireMesh> cube_wire_mesh;
			std::shared_ptr<render::WireMesh> cylinder_wire_mesh;
		
			//tiles 
			int map_size;
			float tile_size;
			WaveFuncMap wave_map;
			std::vector<char> char_map;
			std::vector<int> vacant_tile;
			std::unordered_map<char, std::shared_ptr<render::Mesh>> tile_meshes;
	};


}