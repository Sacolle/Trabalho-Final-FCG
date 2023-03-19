#pragma once

#include <vector>
#include <unordered_map>
#include <memory>

#include "tiny_obj_loader.h"
#include "stb_image.h"

#include <glad/glad.h>
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include "shader.hpp"

namespace render{
	class Mesh{
		public:
			Mesh(
				GLuint _vao_id,  std::vector<GLuint> _buffer_ids,
				std::unordered_map<std::string, GLuint> _texture_ids,
				std::vector<tinyobj::material_t> mats,
				std::unordered_multimap<GLuint, std::pair<GLuint, GLuint>> ranges);
			~Mesh();
			auto draw(std::shared_ptr<GPUprogram> shader) -> void;
		private:
			auto delete_gpu_data() -> void;

			GLuint vao_id;
			std::vector<GLuint> buffer_ids;
			std::unordered_map<std::string, GLuint> texture_ids;
			//desenvolvimento do código auxiliado pelo colega Vinicius Fritzen
			std::vector<tinyobj::material_t> materials;
			std::unordered_multimap<GLuint, std::pair<GLuint, GLuint>> material_draw_ranges;
	};

	class ParsedTextures{
		public:
			ParsedTextures(){}
			//ParsedTextures(ParsedTextures &t) = delete;
			~ParsedTextures();
			//throws exception with bad data
			auto load_texture_file(const char *filename) -> void;
			//throws exception if no data
			auto load_to_gpu() const -> GLuint;
			auto log_data() const -> void;
		private:
			int width = 0;
			int height = 0;
			int channels = 0;
			unsigned char *data = nullptr;
	};

	class ParsedObjMesh{
		public:
			ParsedObjMesh(){} //default constructor
			~ParsedObjMesh(){} //default desctructor

			//throws exception 
			auto load_obj_file(const char* filename, const char* material_directory) -> void;
			//throws exception if no data was loaded
			auto load_to_gpu() -> std::shared_ptr<Mesh>; //retorna o id do VAO
			auto log_parsed_textures() -> void;
		private:
			std::vector<glm::vec3> verts;
			std::vector<glm::vec3> normals;
			std::vector<glm::vec2> texture_cords;
			std::vector<GLuint> indices;

			std::unordered_map<std::string, ParsedTextures> textures;
			std::vector<tinyobj::material_t> materials;
			std::unordered_multimap<GLuint, std::pair<GLuint, GLuint>> material_draw_ranges;
	};

	class WireMesh{
		public:
			//usa-se static_cast para obter o typo de bbox como int
			WireMesh(int type);
			~WireMesh();

			auto draw() -> void;
		private:
			auto load_to_gpu() -> void; //retorna o id do VAO
			auto delete_gpu_data() -> void;

			bool in_gpu;
			GLuint vao_id;
			std::vector<GLuint> buffer_ids;

			std::vector<GLfloat> verts;
			std::vector<GLuint> indexs;
	};

	template <class T>
	inline void hash_combine(std::size_t& seed, const T& v){
		std::hash<T> hasher;
		seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
	}
	//realmente
	struct vec3_hash : std::unary_function<glm::vec3, std::size_t>{
		std::size_t operator()(glm::vec3 const& e) const {
			std::size_t seed = 0;
			hash_combine(seed,e.x);
			hash_combine(seed,e.y);
			hash_combine(seed,e.z);
			return seed;
		}
	};
	/*
	//podia ser facil, mas nãoooooooooooooo
	struct pair_equal_to : std::binary_function<std::pair<int,int>, std::pair<int,int>, bool>
	{
		bool operator()(std::pair<int,int> const& x, std::pair<int,int> const& y) const
		{
			return (x.first == y.first && x.second == y.second);
		}
	};
	*/
}