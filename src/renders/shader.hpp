#pragma once
#include <glad/glad.h>   // Criação de contexto OpenGL 3.3
#include <unordered_map>

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
			auto set_mtx(const char* atrib, const GLfloat *value) -> void;
			auto set_3floats(const char* atrib, const GLfloat f1, const GLfloat f2, const GLfloat f3) -> void;
			auto set_float(const char* atrib, const GLfloat value) -> void;
			auto set_int(const char* atrib, const GLint value) -> void;
			auto set_bool(const char* atrib, const bool value) -> void;
			inline auto get_prog_id() -> GLuint{ return id; }
		private:
			inline auto get_position(const char* atrib) -> GLint {
				if(uniforms.count(atrib)){
					return uniforms[atrib];
				}else{
					const auto model_uniform = glGetUniformLocation(id, atrib);
					uniforms[atrib] = model_uniform;
					return model_uniform;
				}
			}

			GLuint id;
			std::unordered_map<const char*,GLint> uniforms;
	};
}
