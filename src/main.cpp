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
#define WINDOW_HEIGHT 800
#define WINDOW_WIDTH  800
#define log(text) std::cout << text << std::endl

void print_exception(const std::exception& e, int level);
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void ErrorCallback(int error, const char* description);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
//global values, TODO: organize in a struct
float g_ScreenRatio = 1.0f;
entity::LookAtParameters g_look_at_parameters { 0.0f, 0.0f, 8.0f };
entity::RotationAngles g_angles { 0.0f, 0.0f };
controler::CursorState g_cursor { 0, 0, false};
bool g_Paused = false;

typedef struct {
	float width, height;
} WindowSize;
WindowSize g_windowSize {WINDOW_WIDTH,WINDOW_HEIGHT};
entity::PressedKeys g_keys{false, false, false, false};
void game_loop(GLFWwindow *window, const char *vertex_shader, const char *fragment_shader){

	log("load shaders");
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
	log("load meshes");
	
	render::ParsedObjMesh cube_load_mesh;
	render::ParsedObjMesh pawn_load_mesh;
	render::ParsedObjMesh grass_load_mesh;
 
	std::shared_ptr<render::Mesh> cube_mesh;
	std::shared_ptr<render::Mesh> pawn_mesh;
	std::shared_ptr<render::Mesh> grass_mesh;
	try{
		//teapot_load_mesh.load_obj_file("models/teapot.obj", "models/materials");
		cube_load_mesh.load_obj_file("models/cube.obj", "models/materials");
		pawn_load_mesh.load_obj_file("models/pawn.obj", "models/materials");
		grass_load_mesh.load_obj_file("models/grass_field.obj", "models/materials");

		cube_mesh = cube_load_mesh.load_to_gpu();
		pawn_mesh = pawn_load_mesh.load_to_gpu();

		grass_mesh = grass_load_mesh.load_to_gpu();
	}catch(const std::exception& e){
		print_exception(e,0);
		std::exit(EXIT_FAILURE);
	}
	log("init enities");

	std::shared_ptr<render::WireMesh> cube_wire_mesh(new render::WireMesh(static_cast<int>(entity::BBoxType::Rectangle)));
	std::shared_ptr<render::WireMesh> cylinder_wire_mesh(new render::WireMesh(static_cast<int>(entity::BBoxType::Cylinder)));

	std::shared_ptr<entity::Player> pawn(new entity::Player(glm::vec4(0,0,-6,1),gpu_program, pawn_mesh));
	pawn->set_wire_mesh(cylinder_wire_mesh);
	pawn->set_wire_renderer(wire_renderer);
	pawn->set_bbox_type(entity::BBoxType::Cylinder);
	pawn->set_bbox_size(2.0f,1.0f,2.0f);  
	pawn->set_speed(0.05f);

	std::shared_ptr<entity::Wall> wall(new entity::Wall(glm::vec4(2,0,0.1f,1),gpu_program, cube_mesh));
	wall->set_wire_mesh(cube_wire_mesh);
	wall->set_wire_renderer(wire_renderer);
	
	std::shared_ptr<entity::Entity> grass(new entity::Entity(glm::vec4(0.1f,5.0f,0.1f,1), gpu_program, grass_mesh));

	std::shared_ptr<entity::GameEvent> coin(new entity::GameEvent(entity::GameEventTypes::Point, glm::vec4(-2,0,0.1f,1),gpu_program, cube_mesh));
	coin->set_wire_mesh(cube_wire_mesh);
	coin->set_wire_renderer(wire_renderer);
	coin->set_scale(0.5f,0.5f,0.5f);
	coin->set_bbox_size(0.5f,0.5f,0.5f);

	std::shared_ptr<entity::Enemy> enemy(new entity::Enemy(glm::vec4(-10,0,0.1f,1),gpu_program, pawn_mesh));
	enemy->set_wire_mesh(cylinder_wire_mesh);
	enemy->set_wire_renderer(wire_renderer);
	enemy->set_scale(0.5f,0.5f,0.5f);
	enemy->set_bbox_size(0.8f,0.8f,0.8f);
	enemy->set_speed(0.025f);

	log("inicializando o controler");

	controler::GameLoop game_controler(
		std::unique_ptr<entity::Camera>(new entity::Camera(pawn->get_cords())),
		std::unique_ptr<controler::CollisionMap>(new controler::CollisionMap(100,100,2,2)),
		pawn,
		gpu_program, wire_renderer,
		&g_keys, &g_look_at_parameters,
		&g_angles, &g_cursor,
		&g_ScreenRatio, &g_Paused);

	log("inserindo o inimigo");
	//game_controler.insert_wall(wall);
	//game_controler.insert_game_event(coin);
	//game_controler.insert_enemy(enemy);
	game_controler.insert_background(grass);

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
            ImGui::SliderFloat("phi", &g_look_at_parameters.phi, 0.0f, 3.14f);
            ImGui::SliderFloat("theta", &g_look_at_parameters.theta, 0.0f, 3.14f);
            ImGui::SliderFloat("distance", &g_look_at_parameters.radius, 2.5f, 20.0f);
			ImGui::Checkbox("render bbox", &render_bbox);
			ImGui::End();
		}
	    glClearColor(1.0f, 0.2f, 0.2f, 1.0f); // define a cor de fundo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // pinta os pixels do framebuffer 

		game_controler.update(delta_time);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
}
int main(int argc, char** argv)
{
	log("Init");

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
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "INF01047 - 00333916 - Pedro Henrique Boniatti Colle / 00334087 - Eduardo Dalmás Faé", NULL, NULL);
    if (!window){
        glfwTerminate();
       	std::cerr << "ERROR: glfwCreateWindow() failed.\n" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    //callback para o resize da janela, em q se deve realocar a memoria
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    glfwSetWindowSize(window, WINDOW_WIDTH, WINDOW_HEIGHT); // Forçamos a chamada do callback acima, para definir g_ScreenRatio.
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
        glfwGetCursorPos(window, &g_cursor.x, &g_cursor.y);
        g_cursor.clicked = true;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        g_cursor.clicked = false;
    }
}
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos){
    float dx = xpos - g_cursor.x;
    float dy = ypos - g_cursor.y;

    // Atualizamos as variáveis globais para armazenar a posição atual do
    // cursor como sendo a última posição conhecida do cursor.
    g_cursor.x = xpos;
    g_cursor.y = ypos;

	if(!g_Paused) return;

    g_angles.angleX = dx/(g_windowSize.width/2)  * 2*PI; // Calcula o ângulo rotação horizontal de acordo com a porcentagem da tela movida (máximo = 2*PI)
    g_angles.angleY = dy/(g_windowSize.height/2) * 2*PI; // Calcula o ângulo rotação  vertical  de acordo com a porcentagem da tela movida (máximo = 2*PI)
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
	if(key == GLFW_KEY_P && action == GLFW_PRESS){
		g_Paused = !g_Paused;
		if(g_Paused)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		else
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}
//Função para a redimensão da janela
void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    g_ScreenRatio = (float)width / height;
	g_windowSize.width  = width;  // Salva a largura da tela
    g_windowSize.height = height; // Salva a altura  da tela
}
// Definimos o callback para impressão de erros da GLFW no terminal
void ErrorCallback(int error, const char* description)
{
    std::cerr << "ERROR: GLFW: " << description << std::endl;
}
void print_exception(const std::exception& e, int level)
{
    std::cout << std::string(level, ' ') << "exception: " << e.what() << '\n';
    try {
        std::rethrow_if_nested(e);
    } catch(const std::exception& nestedException) {
        print_exception(nestedException, level+1);
    } catch(...) {}
}