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

//headers do IMGUI
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

//minhas implementações
#include "renders/shader.hpp"
#include "utils/matrix.hpp"
#include "entities/entity.hpp"
#include "entities/camera.hpp"
#include "renders/mesh.hpp"
#include "controlers/collision.hpp"
#include "controlers/gameloop.hpp"


#define PI 3.141592f
#define TARGET_FRAME_RATE 60.0f
#define log(text) std::cout << text << std::endl


void print_exception(const std::exception& e, int level);
auto player_movement() -> float;

void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void ErrorCallback(int error, const char* description);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);

//global values, TODO: organize in a struct
float g_ScreenRatio = 1.0f;
float g_CameraPhi = 0.0f, g_CameraTheta = 0.0f;
float g_LeftMouseButtonPressed;
double g_LastCursorPosX, g_LastCursorPosY;

entity::PressedKeys g_keys{false, false, false, false};

void game_loop(GLFWwindow *window, const char *vertex_shader, const char *fragment_shader){
	//carrega os shaders
	std::shared_ptr<render::GPUprogram> gpu_program;
	std::shared_ptr<render::GPUprogram> wire_renderer;
	try{
		std::shared_ptr<render::GPUprogram> gpu_program_expt (new render::GPUprogram(vertex_shader,fragment_shader));
		std::shared_ptr<render::GPUprogram> wire_renderer_expt (new render::GPUprogram("src/shaders/wire_vertex.glsl","src/shaders/wire_fragment.glsl"));
		gpu_program = gpu_program_expt;
		wire_renderer = wire_renderer_expt;
	}catch(const std::exception& e){
		print_exception(e,0);
		std::exit(EXIT_FAILURE);
	}

	//render::ParsedObjMesh teapot_load_mesh;
	render::ParsedObjMesh cube_load_mesh;
	render::ParsedObjMesh pawn_load_mesh;

	//std::shared_ptr<render::Mesh> teapot_mesh;
	std::shared_ptr<render::Mesh> cube_mesh;
	std::shared_ptr<render::Mesh> pawn_mesh;
	try{
		//teapot_load_mesh.load_obj_file("models/teapot.obj", "models/materials");
		cube_load_mesh.load_obj_file("models/cube.obj", "models/materials");
		pawn_load_mesh.load_obj_file("models/pawn.obj", "models/materials");
		//teapot_mesh = teapot_load_mesh.load_to_gpu();
		cube_mesh = cube_load_mesh.load_to_gpu();
		pawn_mesh = pawn_load_mesh.load_to_gpu();
	}catch(const std::exception& e){
		print_exception(e,0);
		std::exit(EXIT_FAILURE);
	}

	std::shared_ptr<render::WireMesh> cube_wire_mesh(new render::WireMesh(static_cast<int>(entity::BBoxType::Rectangle)));
	std::shared_ptr<render::WireMesh> cylinder_wire_mesh(new render::WireMesh(static_cast<int>(entity::BBoxType::Cylinder)));

	std::shared_ptr<entity::Player> pawn(new entity::Player(glm::vec4(0,0,-6,1),gpu_program, pawn_mesh));
	pawn->set_wire_mesh(cylinder_wire_mesh);
	pawn->set_wire_renderer(wire_renderer);
	pawn->set_bbox_type(entity::BBoxType::Cylinder);
	pawn->set_bbox_size(2.0f,1.0f,2.0f);  
	std::shared_ptr<entity::Enemy> cube(new entity::Enemy(glm::vec4(2,0,0.1f,1),gpu_program, cube_mesh));
	cube->set_wire_mesh(cube_wire_mesh);
	cube->set_wire_renderer(wire_renderer);

	log("inicializando o controler");

	controler::GameLoop game_controler(
		std::unique_ptr<entity::Camera>(new entity::Camera()),
		std::unique_ptr<controler::CollisionMap>(new controler::CollisionMap(100,100,2,2)),
		pawn
	);
	log("inserindo o inimigo");
	game_controler.insert_enemy(cube);

	float phi = 0.0f, theta = 0.0f, distance = 8.0f;
	bool render_bbox = false;
	float last_frame = (float)glfwGetTime();

	log("iniciando o loop de render");
    // Ficamos em um loop infinito, renderizando, até que o usuário feche a janela
    while (!glfwWindowShouldClose(window))
    {
		glfwPollEvents();
		float now = (float)glfwGetTime();
		float delta_time = (now - last_frame) * TARGET_FRAME_RATE;
		last_frame = now;

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		{
			ImGui::Begin("Camera Movment");
            ImGui::SliderFloat("phi", &phi, 0.0f, 3.14f);
            ImGui::SliderFloat("theta", &theta, 0.0f, 3.14f);
            ImGui::SliderFloat("distance", &distance, 2.5f, 20.0f);
			ImGui::Checkbox("render bbox", &render_bbox);
			ImGui::End();
		}
	    glClearColor(1.0f, 0.2f, 0.2f, 1.0f); // define a cor de fundo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // pinta os pixels do framebuffer 

		game_controler.update_camera(phi,theta,distance,delta_time);
		game_controler.update_player(delta_time,g_keys);
		game_controler.render_frame(gpu_program);
		if(render_bbox){
			game_controler.render_bbox(wire_renderer);
		}
  
		ImGui::Render();

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
}
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

    glfwSetMouseButtonCallback(window, MouseButtonCallback); //botao do mouse
    glfwSetCursorPosCallback(window, CursorPosCallback); //movimentar o cursor
    glfwSetKeyCallback(window, KeyCallback); //keypress

	glfwMakeContextCurrent(window); //faz com q as chamadas do OpenGL façam render na janela
	//load das funcões da Glad
    if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)){
        glfwTerminate();
       	std::cerr << "ERROR: gladLoadGLLoader() failed.\n" << std::endl;
        std::exit(EXIT_FAILURE);

	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsClassic();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    glEnable(GL_DEPTH_TEST);

	game_loop(window, argv[1],argv[2]);

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

    float phimax = PI/2;
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
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mod)
{
    // Se o usuário pressionar a tecla ESC, fechamos a janela.
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if(key == GLFW_KEY_W && action == GLFW_PRESS){
		g_keys.w = true;
	}
	if(key == GLFW_KEY_W && action == GLFW_RELEASE){
		g_keys.w = false;
	}
	if(key == GLFW_KEY_S && action == GLFW_PRESS){
		g_keys.s = true;
	}
	if(key == GLFW_KEY_S && action == GLFW_RELEASE){
		g_keys.s = false;
	}
	if(key == GLFW_KEY_D && action == GLFW_PRESS){
		g_keys.d = true;
	}
	if(key == GLFW_KEY_D && action == GLFW_RELEASE){
		g_keys.d = false;
	}
	if(key == GLFW_KEY_A && action == GLFW_PRESS){
		g_keys.a = true;
	}
	if(key == GLFW_KEY_A && action == GLFW_RELEASE){
		g_keys.a = false;
	}
    // O código abaixo implementa a seguinte lógica:
    //   Se apertar tecla X       então g_AngleX += delta;
    //   Se apertar tecla shift+X então g_AngleX -= delta;
    //   Se apertar tecla Y       então g_AngleY += delta;
    //   Se apertar tecla shift+Y então g_AngleY -= delta;
    //   Se apertar tecla Z       então g_AngleZ += delta;
    //   Se apertar tecla shift+Z então g_AngleZ -= delta;
/*
    float delta = 3.141592 / 16; // 22.5 graus, em radianos.
    if (key == GLFW_KEY_X && action == GLFW_PRESS)
    {
        g_AngleX += (mod & GLFW_MOD_SHIFT) ? -delta : delta;
    }

    if (key == GLFW_KEY_Y && action == GLFW_PRESS)
    {
        g_AngleY += (mod & GLFW_MOD_SHIFT) ? -delta : delta;
    }
    if (key == GLFW_KEY_Z && action == GLFW_PRESS)
    {
        g_AngleZ += (mod & GLFW_MOD_SHIFT) ? -delta : delta;
    }
    // Se o usuário apertar a tecla espaço, resetamos os ângulos de Euler para zero.
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    {
        g_AngleX = 0.0f;
        g_AngleY = 0.0f;
        g_AngleZ = 0.0f;
    }

    // Se o usuário apertar a tecla P, utilizamos projeção perspectiva.
    if (key == GLFW_KEY_P && action == GLFW_PRESS)
    {
        g_UsePerspectiveProjection = true;
    }

    // Se o usuário apertar a tecla O, utilizamos projeção ortográfica.
    if (key == GLFW_KEY_O && action == GLFW_PRESS)
    {
        g_UsePerspectiveProjection = false;
    }
    // Se o usuário apertar a tecla H, fazemos um "toggle" do texto informativo mostrado na tela.
    if (key == GLFW_KEY_H && action == GLFW_PRESS)
    {
        g_ShowInfoText = !g_ShowInfoText;
    }
*/
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