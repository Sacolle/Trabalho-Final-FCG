#pragma once

#include<vector>

#include "tiny_obj_loader.h"
#include <glad/glad.h>

namespace render{
	class Mesh{
		public:
			Mesh(const char* filename, const char* material_directory);
			~Mesh();

			auto load_to_gpu() -> GLuint; //retorna o id do VAO
			auto draw() -> bool;
			auto delete_gpu_data() -> void;

			inline auto get_vao_id() -> GLuint { return vao_id; }
			inline auto get_num_indices() -> GLuint { return indexs.size(); }

		private:
			bool in_gpu;
			GLuint vao_id;
			std::vector<GLuint> buffer_ids;

			std::vector<GLfloat> verts;
			std::vector<GLuint> indexs;
			//std::vector<tinyobj::real_t> normals;
			//std::vector<tinyobj::real_t> normals;
	};
}