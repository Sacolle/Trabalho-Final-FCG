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

	class Generator{
		public:
			Generator(std::shared_ptr<render::Shader> gpu_program, std::shared_ptr<render::Shader> wire_renderer);
			~Generator();

			auto generate_map_elements() -> struct MapElements;
			auto generate_enemy(const char* type) -> std::shared_ptr<entity::Enemy>;
		private:
			std::unordered_map<const char*, std::shared_ptr<render::Mesh>> meshes;

			std::shared_ptr<render::Shader> gpu_program;
			std::shared_ptr<render::Shader> wire_renderer;
			std::shared_ptr<render::WireMesh> cube_wire_mesh;
			std::shared_ptr<render::WireMesh> cylinder_wire_mesh;
		
			//tiles 
			int map_size;
			int tile_size;
			WaveFuncMap wave_map;
			std::vector<char> char_map;
			std::vector<int> vacant_tile;
			std::unordered_map<char, std::shared_ptr<render::Mesh>> tile_meshes;
	};


}