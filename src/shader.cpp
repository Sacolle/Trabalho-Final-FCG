#include "shader.hpp"
#include <stdexcept>
#include <exception>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

namespace render{
	Shader::Shader(const char* filename, GLenum type){
		// Criamos um identificador (ID) para este shader
		id = glCreateShader(type);
		try{
			load_shader(filename);
		}catch(...){
			std::throw_with_nested(std::runtime_error("Failed to compile Shader"));
		}
	}
	Shader::~Shader(){
		glDeleteShader(id);
		//std::cout << "deleted the shader" << std::endl;
	}
	void Shader::load_shader(const char* filename){

		std::ifstream file;
		try {
			file.exceptions(std::ifstream::failbit);
			file.open(filename);
		} catch (...) {
			std::string error_msg("Failed to open");
			std::throw_with_nested(std::runtime_error(error_msg + filename));
		}

		std::stringstream shader;
		shader << file.rdbuf();
		std::string str = shader.str();
		const GLchar* shader_string = str.c_str();
		const GLint   shader_string_length = static_cast<GLint>( str.length() );

		// Define o código do shader GLSL, contido na string "shader_string"
		glShaderSource(id, 1, &shader_string, &shader_string_length);
		glCompileShader(id);// Compila o código do shader GLSL

		//FIXME: o programa vai gerar erro com uma warning
		GLint compiled_ok;
		glGetShaderiv(id, GL_COMPILE_STATUS, &compiled_ok);
		if(!compiled_ok){
			std::string error_msg("Failed to compile Shader program in");
			std::throw_with_nested(std::runtime_error(error_msg + filename));
		}
	}

	GPUprogram::GPUprogram(const char* vertex_filename,const char* frag_filename){
		Shader *vertex_shader;
		Shader *fragment_shader;
		try{
			vertex_shader = new Shader(vertex_filename,GL_VERTEX_SHADER);
			fragment_shader = new Shader(frag_filename,GL_FRAGMENT_SHADER);
		}catch(...){
			std::throw_with_nested("failed to generate shaders at GPU compile level");
		}


		id = glCreateProgram();
		glAttachShader(id, vertex_shader->get_shader_id());
		glAttachShader(id, fragment_shader->get_shader_id());

		glLinkProgram(id);
		//a esse ponto pode-se deletar os shaders
		delete vertex_shader;
		delete fragment_shader;

		GLint linked_ok = GL_FALSE; //verifica se linkou corretamente
		glGetProgramiv(id, GL_LINK_STATUS, &linked_ok);
		if(linked_ok == GL_FALSE){
			std::throw_with_nested(std::runtime_error("Failed to Link the GPU program"));
		}
	}
	GPUprogram::~GPUprogram(){
		glDeleteProgram(id);
	}

	auto GPUprogram::set_uniform_value(const char* atrib, const GLfloat *value) -> void{
		GLint model_uniform;
		//checa se o atrib existe no mapa
		if(uniforms.count(atrib)){
			model_uniform = uniforms[atrib];
		}else{
			model_uniform = glGetUniformLocation(id, atrib);
			uniforms[atrib] = model_uniform;
		}
		glUniformMatrix4fv(model_uniform, 1 , GL_FALSE , value);
	}
 	auto GPUprogram::use_prog() -> void {
		glUseProgram(id);
	}
}


/*

//TODO: implementar o log de para falha no linker
GLuint CreateGpuProgram(GLuint vertex_shader_id, GLuint fragment_shader_id)
{
    if ( linked_ok == GL_FALSE )
    {
        GLint log_length = 0;
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_length);

        // Alocamos memória para guardar o log de compilação.
        // A chamada "new" em C++ é equivalente ao "malloc()" do C.
        GLchar* log = new GLchar[log_length];

        glGetProgramInfoLog(program_id, log_length, &log_length, log);

        std::string output;

        output += "ERROR: OpenGL linking of program failed.\n";
        output += "== Start of link log\n";
        output += log;
        output += "\n== End of link log\n";

        // A chamada "delete" em C++ é equivalente ao "free()" do C
        delete [] log;

        fprintf(stderr, "%s", output.c_str());
    }

    // Retornamos o ID gerado acima
    return program_id;
}

//TODO: implementar o log de para falha na compilação dos shaders
void LoadShader(const char* filename, GLuint shader_id)
{
    GLint log_length = 0;
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);
    GLchar* log = new GLchar[log_length];
    glGetShaderInfoLog(shader_id, log_length, &log_length, log);
    if ( log_length != 0 ) {
        std::string  output;
        if ( !compiled_ok ) {
            output += "ERROR: OpenGL compilation of \"";
            output += filename;
            output += "\" failed.\n";
            output += "== Start of compilation log\n";
            output += log;
            output += "== End of compilation log\n";
        }
        else{
            output += "WARNING: OpenGL compilation of \"";
            output += filename;
            output += "\".\n";
            output += "== Start of compilation log\n";
            output += log;
            output += "== End of compilation log\n";
        }
        fprintf(stderr, "%s", output.c_str());
    }
    delete [] log;
}
*/