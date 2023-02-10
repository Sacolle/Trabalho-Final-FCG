#pragma once

#include <vector>

#include "tiny_obj_loader.h"
#include <glad/glad.h>
#include <glm/vec3.hpp>

namespace render{
	class Mesh{
		public:
			virtual ~Mesh();
			//funções virtuais puras que devem ser implementadas
			virtual auto load_to_gpu() -> GLuint = 0; 
			virtual auto draw() -> void = 0;

			auto delete_gpu_data() -> void;
		
		protected:
			bool in_gpu;
			GLuint vao_id;
			std::vector<GLuint> buffer_ids;

	};

	class ObjMesh : public Mesh{
		public:
			ObjMesh(const char* filename, const char* material_directory);
			virtual ~ObjMesh();

			auto load_to_gpu() -> GLuint; //retorna o id do VAO
			auto draw() -> void;
			//auto delete_gpu_data() -> void;

			inline auto get_vao_id() -> GLuint { return vao_id; }
			inline auto get_num_indices() -> GLuint { return indexs.size(); }

		private:
			std::vector<GLfloat> verts;
			std::vector<GLuint> indexs;
			//std::vector<tinyobj::real_t> normals;
			//std::vector<tinyobj::real_t> normals;
	};
	class WireMesh : public Mesh{
		public:
			WireMesh(glm::vec3 pos, float theta, float phi, float radius, float height, float scale);
			virtual ~WireMesh();

			auto load_to_gpu() -> GLuint; //retorna o id do VAO
			auto draw() -> void;
		private:
			std::vector<GLfloat> verts;
			std::vector<GLuint> indexs;
	};
}