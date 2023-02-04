#include <cmath>
#include <cstdio>
#include <cstdlib>

// Headers abaixo são específicos de C++
#include <vector>
#include <iostream>

// Headers das bibliotecas OpenGL
#include <glad/glad.h>   // Criação de contexto OpenGL 3.3
#include <GLFW/glfw3.h>  // Criação de janelas do sistema operacional
#include <glm/gtc/type_ptr.hpp>

//headers do IMGUI
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

//minhas implementações
#include "shader.hpp"
#include "vertex.hpp"
#include "matrix.hpp"
#include "camera.hpp"
#include "mesh.hpp"

auto make_cube() -> render::VertexArrayObj*; // Constrói triângulos para renderizaçãO
auto make_plane() -> render::VertexArrayObj*; // Constrói triângulos para renderizaçãO

void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void ErrorCallback(int error, const char* description);
void print_exception(const std::exception& e, int level);

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);

float g_ScreenRatio = 1.0f;
float g_CameraPhi = 0.0f, g_CameraTheta = 0.0f;
float g_LeftMouseButtonPressed;
double g_LastCursorPosX, g_LastCursorPosY;

/*TODO:
	* Implementar a camera de forma rudimentar e fazer display em perspectiva de um objeto 3d
		* Gerar uma classe base de coordenadas
		* Abstrair a camera para uma classe
*/

int main(int argc, char** argv)
{
	if(argc < 3){
       	std::cerr << "Not enought args, exepected 3.\n" << std::endl;
		std::exit(EXIT_FAILURE);
	}
    //Init da lib GLFW
    if (!glfwInit()){
       	std::cerr << "ERROR: glfwInit() failed.\n" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    glfwSetErrorCallback(ErrorCallback); //callback de erros da GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //use-se OpenGL versão 3.3 (ou superior)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //usa-se apenas as funções modernas de OpenGL(perfil "core").
    //cria uma janela 800x800
    GLFWwindow* window = glfwCreateWindow(800, 800, "INF01047 - 00333916 - Pedro Henrique Boniatti Colle", NULL, NULL);
    if (!window){
        glfwTerminate();
       	std::cerr << "ERROR: glfwCreateWindow() failed.\n" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    //callback para o resize da janela, em q se deve realocar a memoria
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    glfwSetWindowSize(window, 800, 800); // Forçamos a chamada do callback acima, para definir g_ScreenRatio.

    //glfwSetMouseButtonCallback(window, MouseButtonCallback); //botao do mouse
    //glfwSetCursorPosCallback(window, CursorPosCallback); //movimentar o cursor
    
	glfwMakeContextCurrent(window); //faz com q as chamadas do OpenGL façam render na janela
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress); //load das funcões da Glad

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsClassic();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

	//carrega os shaders
	render::GPUprogram *gpu_program;
	try{
		gpu_program = new render::GPUprogram(argv[1],argv[2]);	
	}catch(const std::exception& e){
		print_exception(e,0);
		std::exit(EXIT_FAILURE);
	}

    glEnable(GL_DEPTH_TEST);
	auto camera = new entity::camera(true);
    //auto vao = make_plane();

	render::Mesh *mesh;
	try{
		mesh = new render::Mesh("models/teapot.obj", "models/materials");
	}catch(const std::exception& e){
		print_exception(e,0);
		std::exit(EXIT_FAILURE);
	}

	auto vao = mesh->load_to_gpu();
	auto num_verts = mesh->get_num_indices();
	auto model = mtx::indentity() * 0.5f;

	float phi = 0, theta = 0, distance = 2.5f;
    // Ficamos em um loop infinito, renderizando, até que o usuário feche a janela
    while (!glfwWindowShouldClose(window))
    {
		glfwPollEvents();
  
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		{
			ImGui::Begin("Cam");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("Move the camera");               // Display some text (you can use a format strings too)

            ImGui::SliderFloat("phi", &phi, 0.0f, 3.14f);
            ImGui::SliderFloat("theta", &theta, 0.0f, 3.14f);
            ImGui::SliderFloat("distance", &distance, 2.5f, 10.0f);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
		}

		ImGui::Render();
  
        glClearColor(1.0f, 0.2f, 0.2f, 1.0f); // define a cor de fundo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // pinta os pixels do framebuffer 


		gpu_program->use_prog();
		camera->update_position(phi,theta,distance);
		
		//associated with the camera
		gpu_program->set_uniform_value("view",camera->get_view_ptr());
		gpu_program->set_uniform_value("projection",camera->get_projection_ptr());
		//associated with the model
		gpu_program->set_uniform_value("model",glm::value_ptr(model));

		//vao->bind();
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES,num_verts,GL_UNSIGNED_INT,(void*)0);
		glBindVertexArray(0);

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
	//manual clear dos objetos
	delete camera;
	delete mesh;
	delete gpu_program;

    // Finalizamos o uso dos recursos do sistema operacional
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

auto make_cube() -> render::VertexArrayObj* {
	GLfloat verts[] = {
        -0.5f,  0.5f,  0.5f, 1.0f,
        -0.5f, -0.5f,  0.5f, 1.0f,
         0.5f, -0.5f,  0.5f, 1.0f,
         0.5f,  0.5f,  0.5f, 1.0f,
        -0.5f,  0.5f, -0.5f, 1.0f,
        -0.5f, -0.5f, -0.5f, 1.0f,
         0.5f, -0.5f, -0.5f, 1.0f,
         0.5f,  0.5f, -0.5f, 1.0f,
	};
	GLfloat colors[] = {
        1.0f, 0.5f, 0.0f, 1.0f,
        1.0f, 0.5f, 0.0f, 1.0f,
        0.0f, 0.5f, 1.0f, 1.0f,
        0.0f, 0.5f, 1.0f, 1.0f,
        1.0f, 0.5f, 0.0f, 1.0f,
        1.0f, 0.5f, 0.0f, 1.0f,
        0.0f, 0.5f, 1.0f, 1.0f,
        0.0f, 0.5f, 1.0f, 1.0f,
	};
	GLubyte indices[] = {
        0, 1, 2, // triângulo 1 
        7, 6, 5, // triângulo 2 
        3, 2, 6, // triângulo 3 
        4, 0, 3, // triângulo 4 
        4, 5, 1, // triângulo 5 
        1, 5, 6, // triângulo 6 
        0, 2, 3, // triângulo 7 
        7, 5, 4, // triângulo 8 
        3, 6, 7, // triângulo 9 
        4, 3, 7, // triângulo 10
        4, 1, 0, // triângulo 11
        1, 6, 2, // triângulo 12
	};
	//TODO: converter isso para openGL basico de novo
	auto vao = new render::VertexArrayObj();
	vao->bind();

	auto vbo_verts = new render::VertexBufferObj(GL_ARRAY_BUFFER);
	vbo_verts->bind();
	vbo_verts->load_data(sizeof(verts),verts);
	vbo_verts->atrib_pointer(0,4,GL_FLOAT);
	vbo_verts->unbind();

	vao->attach_verts(vbo_verts);

	auto vbo_colors = new render::VertexBufferObj(GL_ARRAY_BUFFER);
	vbo_colors->bind();
	vbo_colors->load_data(sizeof(colors),colors);
	vbo_colors->atrib_pointer(1,4,GL_FLOAT);
	vbo_colors->unbind();

	vao->attach_colors(vbo_colors);

	auto vbo_indices = new render::VertexBufferObj(GL_ELEMENT_ARRAY_BUFFER);
	vbo_indices->bind();
	vbo_indices->load_data(sizeof(indices),indices);
	//não se realiza o unbind dos indices
	vao->attach_indices(vbo_indices);

	vao->unbind();
	return vao;
}

auto make_plane() -> render::VertexArrayObj* {
	GLfloat verts[] = {
         1.0f,  0.0f,  1.0f,
         1.0f,  0.0f, -1.0f,
        -1.0f,  0.0f,  1.0f,
        -1.0f,  0.0f, -1.0f,
	};
	GLfloat colors[] = {
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
	};
	GLubyte indices[] = {
        0, 1, 2, // triângulo 1 
        1, 2, 3, // triângulo 2 
	};
	//TODO: converter isso para openGL basico de novo
	auto vao = new render::VertexArrayObj();
	vao->bind();

	auto vbo_verts = new render::VertexBufferObj(GL_ARRAY_BUFFER);
	vbo_verts->bind();
	vbo_verts->load_data(sizeof(verts),verts);
	vbo_verts->atrib_pointer(0,3,GL_FLOAT);
	vbo_verts->unbind();

	vao->attach_verts(vbo_verts);

	auto vbo_colors = new render::VertexBufferObj(GL_ARRAY_BUFFER);
	vbo_colors->bind();
	vbo_colors->load_data(sizeof(colors),colors);
	vbo_colors->atrib_pointer(1,4,GL_FLOAT);
	vbo_colors->unbind();

	vao->attach_colors(vbo_colors);

	auto vbo_indices = new render::VertexBufferObj(GL_ELEMENT_ARRAY_BUFFER);
	vbo_indices->bind();
	vbo_indices->load_data(sizeof(indices),indices);
	//não se realiza o unbind dos indices
	vao->attach_indices(vbo_indices);

	vao->unbind();
	return vao;
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods){
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        glfwGetCursorPos(window, &g_LastCursorPosX, &g_LastCursorPosY);
        g_LeftMouseButtonPressed = true;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        g_LeftMouseButtonPressed = false;
    }
}
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos){
    if (!g_LeftMouseButtonPressed)
        return;

    float dx = xpos - g_LastCursorPosX;
    float dy = ypos - g_LastCursorPosY;

    g_CameraTheta -= 0.01f*dx;
    g_CameraPhi   -= 0.01f*dy;

    float phimax = 3.141592f/2;
    float phimin = -phimax;

    if (g_CameraPhi > phimax)
        g_CameraPhi = phimax;

    if (g_CameraPhi < phimin)
        g_CameraPhi = phimin;

    // Atualizamos as variáveis globais para armazenar a posição atual do
    // cursor como sendo a última posição conhecida do cursor.
    g_LastCursorPosX = xpos;
    g_LastCursorPosY = ypos;
}
//Função para a redimensão da janela
void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    g_ScreenRatio = (float)width / height;
}
// Definimos o callback para impressão de erros da GLFW no terminal
void ErrorCallback(int error, const char* description)
{
    std::cerr << "ERROR: GLFW: " << description << std::endl;

}
void print_exception(const std::exception& e, int level)
{
    std::cerr << std::string(level, ' ') << "exception: " << e.what() << '\n';
    try {
        std::rethrow_if_nested(e);
    } catch(const std::exception& nestedException) {
        print_exception(nestedException, level+1);
    } catch(...) {}
}