#pragma once

#include <memory>

#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.hpp"
#include "mesh.hpp"

namespace render{
	class Renderable{
		public:
			//full constructor
			Renderable(std::shared_ptr<render::GPUprogram> gpu_program,
				std::shared_ptr<render::Mesh> mesh,
				std::shared_ptr<render::GPUprogram> wire_renderer,
				std::shared_ptr<render::WireMesh> wire_mesh);
			//simpler 
			Renderable(std::shared_ptr<render::GPUprogram> gpu_program,
				std::shared_ptr<render::Mesh> mesh
			);
			//default
			Renderable();
 
			auto set_gpu_program(std::shared_ptr<render::GPUprogram> _gpu_program) -> void;
			auto set_mesh(std::shared_ptr<render::Mesh> _mesh) -> void;
			auto set_wire_renderer(std::shared_ptr<render::GPUprogram> _wire_renderer) -> void;
			auto set_wire_mesh(std::shared_ptr<render::WireMesh> _wire_mesh) -> void;

			auto draw(const glm::mat4 &model_transform) -> void;
			auto draw_wire(const glm::mat4 &model_transform) -> void;
		private:
			//for rendering the mesh
			std::shared_ptr<render::GPUprogram> gpu_program;
			std::shared_ptr<render::Mesh> mesh;

			//for rendering the wire_wesh (vizualizing the bbox)
			std::shared_ptr<render::GPUprogram> wire_renderer;
			std::shared_ptr<render::WireMesh> wire_mesh;
	};
}