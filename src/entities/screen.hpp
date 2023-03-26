#pragma once

#include <vector>
#include <memory>

#include "../renders/renderable.hpp"
#include "geometry.hpp"

namespace entity {
	enum class MenuOptions{
		Play,
		Retry,
		Options,
		Exit,
		RickRoll,
		None
	};
	class Node;


	//handles nodes
	class Screen : public Geometry, public render::Renderable {
		public:
			//initialize the geometry and renderable
			Screen(glm::vec4 cords, 
				std::shared_ptr<render::GPUprogram> gpu_program,
				std::shared_ptr<render::Mesh> mesh): Geometry(cords), render::Renderable(gpu_program, mesh){} 

			//calls its nodes draw methods
			auto draw_nodes() -> void;

			auto click_action(double x, double y, float screen_width, float screen_height) -> MenuOptions;

			auto insert_nodes(std::shared_ptr<Node> noh, float b, float t, float l, float r, float screen_width, float screen_height) -> void;
		private:


			std::vector<std::shared_ptr<Node>> nodes;

	};

	class Node : public Geometry, public render::Renderable {
		public:
			//initialize the geometry and renderable
			Node(glm::vec4 cords, 
				std::shared_ptr<render::GPUprogram> gpu_program,
				std::shared_ptr<render::Mesh> mesh,
				MenuOptions action):
				Geometry(cords), render::Renderable(gpu_program, mesh), action(action){} 

			auto set_bounds(float b, float t, float l, float r, float screen_width, float screen_height) -> void;

			auto click_inside(float x, float y) -> bool;
			inline auto get_action() -> MenuOptions { return action; }
		private:
			float bottom;
			float top;
			float left;
			float right;

			MenuOptions action;
	};
}