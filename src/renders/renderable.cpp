#include "renderable.hpp"

namespace render{
	/**************************
		Entity implementation
	***************************/
	Renderable::Renderable(std::shared_ptr<render::GPUprogram> gpu_program,
		std::shared_ptr<render::Mesh> mesh,
		std::shared_ptr<render::GPUprogram> wire_renderer,
		std::shared_ptr<render::WireMesh> wire_mesh):
		gpu_program(gpu_program), mesh(mesh), wire_renderer(wire_renderer), wire_mesh(wire_mesh){}

	Renderable::Renderable(std::shared_ptr<render::GPUprogram> gpu_program,
		std::shared_ptr<render::Mesh> mesh): gpu_program(gpu_program), mesh(mesh){}
	

	auto Renderable::set_gpu_program(std::shared_ptr<render::GPUprogram> _gpu_program) -> void {
		gpu_program = _gpu_program;
	}
	auto Renderable::set_mesh(std::shared_ptr<render::Mesh> _mesh) -> void {
		mesh = _mesh;
	}
	auto Renderable::set_wire_renderer(std::shared_ptr<render::GPUprogram> _wire_renderer) -> void {
		wire_renderer = _wire_renderer;
	}
	auto Renderable::set_wire_mesh(std::shared_ptr<render::WireMesh> _wire_mesh) -> void {
		wire_mesh = _wire_mesh;
	}

	auto Renderable::draw(const glm::mat4 &model_transform) -> void {
		gpu_program->set_mtx("model_transform",glm::value_ptr(model_transform));
		mesh->draw(gpu_program);		
	}
	auto Renderable::draw_wire(const glm::mat4 &model_transform) -> void {
		if(wire_mesh == nullptr || wire_renderer == nullptr){
			return;
		}
		wire_renderer->set_mtx("transform", glm::value_ptr(model_transform));
		wire_mesh->draw();		
	}
} 
