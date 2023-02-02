#include "vertex.hpp"
#include <iostream>

namespace render{
	VertexBufferObj::VertexBufferObj(GLenum my_type){
		type = my_type;
		GLuint buff_id;
		glGenBuffers(1, &buff_id);
		id = buff_id;
	}
	VertexBufferObj::~VertexBufferObj(){
		glDeleteBuffers(1,&id);
	}
	auto VertexBufferObj::bind() -> void { glBindBuffer(type,id); }
	auto VertexBufferObj::unbind() -> void { glBindBuffer(type,0); }

	auto VertexBufferObj::load_data(GLsizeiptr size, const GLvoid* data) -> void{
		glBufferData(type, size, NULL, GL_STATIC_DRAW);
		glBufferSubData(type, 0, size,data);
	}
	auto VertexBufferObj::atrib_pointer(GLuint location,GLuint dimensions, GLenum type) -> void{
		glVertexAttribPointer(location, dimensions, type, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(location);
	}

	VertexArrayObj::VertexArrayObj(){
		verts = nullptr;
		colors = nullptr;
		indices = nullptr;

		GLuint buff_id;
		glGenVertexArrays(1, &buff_id);
		id = buff_id;
	}
	VertexArrayObj::~VertexArrayObj(){
		glDeleteBuffers(1,&id); //deleta o VAO
		//deleta os VBOS
		delete verts;
		delete colors;
		delete indices;
	}
	auto VertexArrayObj::attach_verts(render::VertexBufferObj *vert) -> void { verts = vert; }
	auto VertexArrayObj::attach_colors(render::VertexBufferObj *col) -> void { colors = col; }
	auto VertexArrayObj::attach_indices(render::VertexBufferObj *idx)-> void { indices = idx; }
	auto VertexArrayObj::bind() -> void { glBindVertexArray(id); } 
	auto VertexArrayObj::unbind() -> void { glBindVertexArray(0); }
}