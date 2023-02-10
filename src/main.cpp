#include <cmath>
#include <cstdio>
#include <cstdlib>

// Headers abaixo são específicos de C++
#include <vector>
#include <iostream>
#include <memory>

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
#include "matrix.hpp"
#include "entities.hpp"
#include "mesh.hpp"

#define DBG 

#ifdef DBG
#define DRAWDBG(x) if(!x){std::cerr << "DRAW NOT ALLOCATED IN LINE: " << __LINE__ << std::endl; exit(1);}
#else
#define DRAWDBG(x) x;
#endif

void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void ErrorCallback(int error, const char* description);
void print_exception(const std::exception& e, int level);

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);

float g_ScreenRatio = 1.0f;
float g_CameraPhi = 0.0f, g_CameraTheta = 0.0f;
float g_LeftMouseButtonPressed;
double g_LastCursorPosX, g_LastCursorPosY;


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

    glEnable(GL_DEPTH_TEST);

	//scope em que os valores alocados vao ser dropados
	{
	//carrega os shaders
	std::unique_ptr<render::GPUprogram> gpu_program;
	try{
		std::unique_ptr<render::GPUprogram> gpu_program_expt (new render::GPUprogram(argv[1],argv[2]));
		gpu_program = std::move(gpu_program_expt);
	}catch(const std::exception& e){
		print_exception(e,0);
		std::exit(EXIT_FAILURE);
	}
	std::unique_ptr<entity::Camera> camera (new entity::Camera(true));

	std::unique_ptr<render::ObjMesh> teapot;
	std::unique_ptr<render::ObjMesh> plane;
	try{
		std::unique_ptr<render::ObjMesh> teapot_expt (new render::ObjMesh("models/teapot.obj", "models/materials"));
		std::unique_ptr<render::ObjMesh> plane_expt (new render::ObjMesh("models/plane.obj", "models/materials"));
		teapot = std::move(teapot_expt);
		plane = std::move(plane_expt);
	}catch(const std::exception& e){
		print_exception(e,0);
		std::exit(EXIT_FAILURE);
	}

	teapot->load_to_gpu();
	plane->load_to_gpu();
	auto tea_model = mtx::scale(0.5f,0.5f,0.5f) * mtx::translate(1.0f,1.0f,1.0f);
	mtx::print(tea_model);
	auto plane_model = mtx::indentity();

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
		gpu_program->set_uniform_value("model",glm::value_ptr(tea_model));
		DRAWDBG(teapot->draw());

		gpu_program->set_uniform_value("model",glm::value_ptr(plane_model));
		DRAWDBG(plane->draw());

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
	//manual clear dos objetos
	}  //fim do escopo das alocações com smart pointers
    // Finalizamos o uso dos recursos do sistema operacional
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
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