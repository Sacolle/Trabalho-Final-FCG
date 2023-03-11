#define TINYOBJLOADER_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

#include "mesh.hpp"

#include <exception>
#include <stdexcept>
#include <string>
#include <iostream>
#include <cmath>

#include <glm/gtc/type_ptr.hpp>

#define CIRCLE_DEFINITION 16
#define PI 3.141592f

namespace render{
	/*****************************
		Mesh implementation
	******************************/
	Mesh::Mesh(GLuint _vao_id,  std::vector<GLuint> _buffer_ids,
		std::unordered_map<std::string, GLuint> _texture_ids,
		std::vector<tinyobj::material_t> mats,
		std::unordered_multimap<GLuint, std::pair<GLuint, GLuint>> ranges): 
		vao_id(_vao_id), buffer_ids(_buffer_ids), texture_ids(_texture_ids), materials(mats), material_draw_ranges(ranges){}
	
	Mesh::~Mesh() {
		//delete the alocations on the gpu
		delete_gpu_data();
	}
	auto Mesh::delete_gpu_data() -> void{
		glDeleteBuffers(1,&vao_id);
		glDeleteBuffers(buffer_ids.size(),&buffer_ids[0]);

		//clears the textures allocated
		for(const auto &elem : texture_ids){
			glDeleteTextures(1,&elem.second);
		}
	}
	auto Mesh::draw(std::shared_ptr<GPUprogram> shader) -> void {
		glBindVertexArray(vao_id);
		for(const auto &elem: material_draw_ranges){
			
			const auto mat_id = elem.first; //key of the multimap
			const auto &material = materials.at(mat_id);
			//val of the multimap
			const auto start = elem.second.first;
			const auto amount = elem.second.second;
			
			//change to &material.ambient[0] if performance demands
			shader->set_vec3("Ka", glm::value_ptr(glm::vec3(material.ambient[0],material.ambient[1],material.ambient[2])));
			shader->set_vec3("KdIn", glm::value_ptr(glm::vec3(material.diffuse[0], material.diffuse[1], material.diffuse[2])));
			shader->set_vec3("Ks", glm::value_ptr(glm::vec3(material.specular[0], material.specular[1], material.specular[2])));
			shader->set_float("q", material.shininess);
			if (!material.diffuse_texname.empty() && texture_ids.count(material.diffuse_texname)) {
				const auto texture = texture_ids[material.diffuse_texname];

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, texture);

				shader->set_bool("using_texture", true);
			} else {
				shader->set_bool("using_texture", false);
			}
			glDrawElements(GL_TRIANGLES, amount, GL_UNSIGNED_INT, (void*)(start * sizeof(GLuint)));
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		glBindVertexArray(0);
	}
	auto ParsedObjMesh::load_obj_file(const char* filename, const char* material_directory) -> void{
		tinyobj::ObjReaderConfig reader_config;
		reader_config.mtl_search_path = material_directory; // Path to material files

		tinyobj::ObjReader reader;

		if (!reader.ParseFromFile(filename, reader_config)) {
			std::string error_msg("Erro no parse do arquivo: ");
			error_msg += filename;
			error_msg += "\n Elaborations: \n";
			std::throw_with_nested(std::runtime_error(error_msg + reader.Error()));
		}
		if (!reader.Warning().empty()) {
			std::cout << "TinyObjReader: " << reader.Warning() << std::endl;
		}
		const auto& attrib = reader.GetAttrib();
		const auto& shapes = reader.GetShapes();
		const auto& mats = reader.GetMaterials();

		std::unordered_map<glm::vec3, GLuint, vec3_hash> vertices;

		//for every shape in the .obj
		for (const auto &shape : shapes){
            size_t num_triangles = shape.mesh.num_face_vertices.size();
            GLuint start_idx = indices.size();
			if(shape.mesh.material_ids[0] == -1){
				std::throw_with_nested(std::runtime_error("File has no material"));
			}
			GLuint material_id = shape.mesh.material_ids[0];

			//for every triagle in the shape
            for (size_t t = 0; t < num_triangles; t++) {
                assert(shape.mesh.num_face_vertices[t] == 3);
				//for every vertex in the triangle
				for (size_t v = 0; v < 3; v++){
                    tinyobj::index_t idx = shape.mesh.indices[3 * t + v];

                    const float vx = attrib.vertices[3 * idx.vertex_index + 0];
                    const float vy = attrib.vertices[3 * idx.vertex_index + 1];
                    const float vz = attrib.vertices[3 * idx.vertex_index + 2];
					glm::vec3 position(vx, vy, vz);

					//if the vertex at that position has not yet being processed
                    if (vertices.count(position) == 0) {
						const auto position_indice = static_cast<GLuint>(verts.size());
                        vertices[position] = position_indice;
						verts.push_back(position);
						indices.push_back(position_indice);
                    }else{
						//if it already computed the point, it adds the indice and goes to next loop interation
						indices.push_back(vertices[position]);
						continue;
					}

                    if (idx.normal_index != -1) {
                        const float nx = attrib.normals[3 * idx.normal_index + 0];
                        const float ny = attrib.normals[3 * idx.normal_index + 1];
                        const float nz = attrib.normals[3 * idx.normal_index + 2];
						normals.push_back(glm::vec3(nx,ny,nz));
                    }else{
						//add a default value to not disilign the vectors for rendering
						normals.push_back(glm::vec3(0,0,0));
					}

                    if (idx.texcoord_index != -1) {
                        const float u = attrib.texcoords[2 * idx.texcoord_index + 0];
                        const float v = attrib.texcoords[2 * idx.texcoord_index + 1];
						texture_cords.push_back(glm::vec2(u,v));
                    }
					texture_cords.push_back(glm::vec2(0,0));
				}
			}
			if(material_id > materials.size()){
				std::throw_with_nested(std::runtime_error("Material not Found, id: " + std::to_string(material_id)));
			}
			material_draw_ranges.emplace(material_id, std::make_pair(start_idx, indices.size()));

			//load texture if it has a name and if it was not loadded before
			const auto &mat_texture_name = mats.at(material_id).diffuse_texname;
			if(!mat_texture_name.empty() && (textures.count(mat_texture_name) == 0)){
				ParsedTextures texture;
				try{
					texture.load_texture_file(mat_texture_name.c_str());
				}catch(...){
					std::string error("Attempt to parse ");
					std::throw_with_nested(std::runtime_error(error + mat_texture_name + " failed."));
				}
				textures[mat_texture_name] = texture;
			}
		}
		materials = std::move(mats);
	}
	auto ParsedObjMesh::load_to_gpu() -> std::shared_ptr<Mesh>{
		if(verts.empty() || indices.empty()){
			std::throw_with_nested(std::runtime_error("atempt to load data when no data was present"));
		}
		std::unordered_map<std::string, GLuint> texture_ids;
		//load the textures
		for(const auto &pair : textures){
			const auto &texture_name = pair.first;
			const auto &texture = pair.second;
			try{
				texture_ids[texture_name] = texture.load_to_gpu();
			}catch(...){
				std::string error("Failed to load ");
				std::throw_with_nested(std::runtime_error(error + texture_name +  " to GPU."));
			}
		}
		//vao settup
		GLuint vao_id;
		std::vector<GLuint> buffer_ids;

		glGenVertexArrays(1,&vao_id);
		glBindVertexArray(vao_id);

		//verts
		GLuint vbo_verts;
		glGenBuffers(1,&vbo_verts);
		glBindBuffer(GL_ARRAY_BUFFER,vbo_verts);
		glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(glm::vec3),NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, verts.size() * sizeof(glm::vec3),&verts[0]);

		// "(location = 0)" em "shader_vertex.glsl"
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, 0);
		glEnableVertexAttribArray(0);	

		glBindBuffer(GL_ARRAY_BUFFER,0);
		buffer_ids.push_back(vbo_verts);

		//normals
		GLuint vbo_normals;
		glGenBuffers(1,&vbo_normals);
		glBindBuffer(GL_ARRAY_BUFFER,vbo_normals);
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3),NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, normals.size() * sizeof(glm::vec3),&normals[0]);

		// "(location = 1)" em "shader_vertex.glsl"
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, 0);
		glEnableVertexAttribArray(1);	

		glBindBuffer(GL_ARRAY_BUFFER,0);
		buffer_ids.push_back(vbo_normals);

		//texture cords
		GLuint vbo_text_cords;
		glGenBuffers(1,&vbo_text_cords);
		glBindBuffer(GL_ARRAY_BUFFER,vbo_text_cords);
		glBufferData(GL_ARRAY_BUFFER, texture_cords.size() * sizeof(glm::vec2),NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, texture_cords.size() * sizeof(glm::vec3),&texture_cords[0]);

		// "(location = 2)" em "shader_vertex.glsl"
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float)*2, 0);
		glEnableVertexAttribArray(2);	

		glBindBuffer(GL_ARRAY_BUFFER,0);
		buffer_ids.push_back(vbo_text_cords);

		//indices
		GLuint indices_id;
		glGenBuffers(1, &indices_id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(GLuint), &indices[0]);

		buffer_ids.push_back(indices_id);

		glBindVertexArray(0);

		return std::shared_ptr<Mesh>(new Mesh(vao_id, buffer_ids, texture_ids, materials, material_draw_ranges));
	}


	ParsedTextures::~ParsedTextures(){
		stbi_image_free(data);
	}
	//throws exception with bad data
	auto ParsedTextures::load_texture_file(const char *filename) -> void {
		stbi_set_flip_vertically_on_load(true);
		data = stbi_load(filename, &width, &height, &channels, 0);
		if(data == nullptr){
			std::throw_with_nested(std::runtime_error("Error on Texture Load"));
		}
	}
	//throws exception if no data
	auto ParsedTextures::load_to_gpu() const -> GLuint {
		if(data == nullptr){
			std::throw_with_nested(std::runtime_error("No texture data to load to GPU"));
		}
		GLuint texture_id;
		glGenTextures(1, &texture_id);
		glBindTexture(GL_TEXTURE_2D, texture_id);
		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		//unbind
		glBindTexture(GL_TEXTURE_2D, 0);

		return texture_id;
	}

	/*****************************
		WireMesh implementation
	******************************/
	WireMesh::WireMesh(int type){
		switch (type){
		case 0:
			verts = std::vector<GLfloat>{
				-1.0f, +1.0f, -1.0f,
				+1.0f, +1.0f, -1.0f,
				-1.0f, +1.0f, +1.0f,
				+1.0f, +1.0f, +1.0f,
				-1.0f, -1.0f, -1.0f,
				+1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f, +1.0f,
				+1.0f, -1.0f, +1.0f,
			};
			
			indexs = std::vector<GLuint>{
				0, 1, 0, 2, 0, 4,
				3, 1, 3, 2, 3, 7,
				6, 2, 6, 4, 6, 7,
				5, 1, 5, 4, 5, 7
			};
			break;
		//cilinder
		//center = 0, 0, 0
		//r = 1	(diameter = 2)
		//height = 1 (total = 2)
		case 1:
		{
			const float segment = 2*PI/CIRCLE_DEFINITION;
			//top verts
			for(int i = 0; i < CIRCLE_DEFINITION; i++){
				verts.push_back(cosf(segment*i)); //x
				verts.push_back(+1.0f); //y
				verts.push_back(sinf(segment*i)); //z
			}
			//bottom verts
			for(int i = 0; i < CIRCLE_DEFINITION; i++){
				verts.push_back(cosf(segment*i)); //x
				verts.push_back(-1.0f); //y
				verts.push_back(sinf(segment*i)); //z
			}
			//top indexes
			for(int i = 0; i < CIRCLE_DEFINITION; i+=2){
				//clock wise
				indexs.push_back(i);
				indexs.push_back(i + 1);
				//counter clock wise
				indexs.push_back(i);
				indexs.push_back(i - 1 < 0 ? CIRCLE_DEFINITION - 1 : i - 1); //if 0 then loop around
				//top
				indexs.push_back(i);
				indexs.push_back(i + CIRCLE_DEFINITION);
			}
			//bottom indexes
			for(int i = CIRCLE_DEFINITION + 1; i < 2*CIRCLE_DEFINITION; i+=2){
				//clock wise
				indexs.push_back(i);
				indexs.push_back(i + 1 == 2*CIRCLE_DEFINITION ? CIRCLE_DEFINITION : i + 1);
				//counter clock wise
				indexs.push_back(i);
				indexs.push_back(i - 1); //if 0 then loop around
				//top
				indexs.push_back(i);
				indexs.push_back(i - CIRCLE_DEFINITION);
			}
			break;
		}
		case 2:
			std::throw_with_nested(std::runtime_error("tipo de BBox Sphere não implementado"));
			break;
		default:
			std::throw_with_nested(std::runtime_error("tipo de BBox não reconhecido"));
			break;
		}
	}
	WireMesh::~WireMesh(){ }

	auto WireMesh::load_to_gpu() -> void{
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