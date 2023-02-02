#pragma once

#include <glad/glad.h>   // Criação de contexto OpenGL 3.3

namespace render{
	//TODO: compositionar a leitura de .obj files nessa classe depois

	class VertexBufferObj{
		public:
			VertexBufferObj(GLenum type);
			~VertexBufferObj();
			auto bind() -> void;
			auto unbind() -> void;

			auto load_data(GLsizeiptr size, const GLvoid* data) -> void;
			auto atrib_pointer(GLuint location,GLuint dimensions, GLenum type) -> void;

			inline auto get_id() -> GLuint { return id; }
		private:
			GLuint id;
			GLenum type;
	};

	class VertexArrayObj{
		public:
			VertexArrayObj();
			~VertexArrayObj();
			
			auto attach_verts(render::VertexBufferObj *vert)	-> void;
			auto attach_colors(render::VertexBufferObj *col)	-> void;
			auto attach_indices(render::VertexBufferObj *idx)   -> void;

			auto bind() -> void; 
			auto unbind() -> void;
			inline auto get_id() -> GLuint { return id; }

		private:
			render::VertexBufferObj *verts;	
			render::VertexBufferObj *colors;	
			render::VertexBufferObj *indices;	
			GLuint id;	
	};

}
