#pragma once
#include <glad/glad.h>   // Criação de contexto OpenGL 3.3
#include <map>

namespace render{
	//Shader class throws exepctions
	class Shader{
		public:
			Shader(const char* filename, GLenum type);
			~Shader();
			inline auto get_shader_id() -> GLuint{ return id; }
		private:
			void load_shader(const char* filename);	//throws exceptions
			GLuint id;
	};
	//GPUprogram class throws exepctions
	class GPUprogram{
		public:
			GPUprogram(const char* vertex_filename,const char* frag_filename);
			~GPUprogram();
			auto use_prog() -> void;
			auto set_uniform_value(const char* atrib, const GLfloat *value) -> void;
			inline auto get_prog_id() -> GLuint{ return id; }
		private:
			GLuint id;
			std::map<const char*,GLint> uniforms;
	};
}
