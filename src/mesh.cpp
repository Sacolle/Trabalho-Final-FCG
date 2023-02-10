#define TINYOBJLOADER_IMPLEMENTATION
#include "mesh.hpp"

#include <exception>
#include <stdexcept>
#include <string>
#include <iostream>

namespace render{

	/*****************************
		Mesh implementation
	******************************/
	Mesh::~Mesh(){
		delete_gpu_data();
	}
	auto Mesh::delete_gpu_data() -> void{
		if(in_gpu){
			glDeleteBuffers(1,&vao_id);
			glDeleteBuffers(buffer_ids.size(),&buffer_ids[0]);
		}
		in_gpu = false;
	}

	/*****************************
		ObjMesh implementation
	******************************/
	ObjMesh::ObjMesh(const char* filename, const char* material_directory){
		tinyobj::ObjReaderConfig reader_config;
		reader_config.mtl_search_path = material_directory; // Path to material files

		tinyobj::ObjReader reader;

		if (!reader.ParseFromFile(filename, reader_config)) {
			std::string error_msg("Erro no parse do arquivo: ");
			error_msg += filename;
			error_msg += " Elaborations: \n";
			std::throw_with_nested(std::runtime_error(error_msg + reader.Error()));
		}
		if (!reader.Warning().empty()) {
			std::cout << "TinyObjReader: " << reader.Warning() << std::endl;
		}

		auto& attrib = reader.GetAttrib();
		auto& shapes = reader.GetShapes();

		//TODO: change for when shapes > 1
		for(auto idx : shapes[0].mesh.indices){
			indexs.push_back(GLuint(idx.vertex_index));
		}
		verts = attrib.vertices;
	}
	ObjMesh::~ObjMesh(){
	}
	//retorna o id do VAO
	auto ObjMesh::load_to_gpu() -> GLuint{
		glGenVertexArrays(1,&vao_id);
		glBindVertexArray(vao_id);

		//verts
		GLuint vbo_verts;
		glGenBuffers(1,&vbo_verts);
		glBindBuffer(GL_ARRAY_BUFFER,vbo_verts);
		glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(GLfloat),NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, verts.size() * sizeof(GLfloat),&verts[0]);

		GLuint location = 0; // "(location = 0)" em "shader_vertex.glsl"
		glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(location);	

		glBindBuffer(GL_ARRAY_BUFFER,0);
		buffer_ids.push_back(vbo_verts);

		//indices
		GLuint indices_id;
		glGenBuffers(1, &indices_id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexs.size() * sizeof(GLuint), NULL, GL_STATIC_DRAW);

		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indexs.size() * sizeof(GLuint), &indexs[0]);

		buffer_ids.push_back(indices_id);

		glBindVertexArray(0);
		in_gpu = true;
		return vao_id;
	}
	auto ObjMesh::draw() -> void {
		if(!in_gpu){
			load_to_gpu();
		}
		glBindVertexArray(vao_id);
		glDrawElements(GL_TRIANGLES,indexs.size(),GL_UNSIGNED_INT,(void*)0);
		glBindVertexArray(0);
	}


	/*****************************
		WireMesh implementation
	******************************/
	WireMesh::WireMesh(glm::vec3 pos, float theta, float phi, float radius, float height, float scale){
		float x = pos.x;
		float y = pos.y;
		float z = pos.z;
		
		verts = std::vector<GLfloat>{
			x - radius, y + height, z - radius,
			x + radius, y + height, z - radius,
			x - radius, y + height, z + radius,
			x + radius, y + height, z + radius,
			x - radius, y - height, z - radius,
			x + radius, y - height, z - radius,
			x - radius, y - height, z + radius,
			x + radius, y - height, z + radius,
		};
		
		indexs = std::vector<GLuint>{
			0, 1, 0, 2, 0, 4,
			3, 1, 3, 2, 3, 7,
			6, 2, 6, 4, 6, 7,
			5, 1, 5, 4, 5, 7
		};
	}
	WireMesh::~WireMesh(){ }
	auto WireMesh::load_to_gpu() -> GLuint{
		glGenVertexArrays(1,&vao_id);
		glBindVertexArray(vao_id);

		//verts
		GLuint vbo_verts;
		glGenBuffers(1,&vbo_verts);
		glBindBuffer(GL_ARRAY_BUFFER,vbo_verts);
		glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(GLfloat),NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, verts.size() * sizeof(GLfloat),&verts[0]);

		GLuint location = 0; // "(location = 0)" em "shader_vertex.glsl"
		glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(location);	

		glBindBuffer(GL_ARRAY_BUFFER,0);
		buffer_ids.push_back(vbo_verts);

		//indices
		GLuint indices_id;
		glGenBuffers(1, &indices_id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexs.size() * sizeof(GLuint), NULL, GL_STATIC_DRAW);

		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indexs.size() * sizeof(GLuint), &indexs[0]);

		buffer_ids.push_back(indices_id);

		glBindVertexArray(0);
		in_gpu = true;
		return vao_id;
	}
	auto WireMesh::draw() -> void{
		if(!in_gpu){
			load_to_gpu();
		}
		glBindVertexArray(vao_id);
		glDrawElements(GL_LINES,indexs.size(),GL_UNSIGNED_INT,(void*)0);
		glBindVertexArray(0);
	}
}