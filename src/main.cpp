#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>
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
#include "entities/screen.hpp"
#include "renders/mesh.hpp"
#include "controlers/collision.hpp"
#include "controlers/gameloop.hpp"

#define PI 3.141592f
#define TARGET_FRAME_RATE 60.0f
#define WINDOW_HEIGHT 800
#define WINDOW_WIDTH  800
#define log(text) std::cout << text << std::endl
#define PHIMAX PI/2
#define PHIMIN 0

auto load_mesh(const char * file) -> std::shared_ptr<render::Mesh>;
auto load_gpu_program(const char * vertex, const char * frag) -> std::shared_ptr<render::GPUprogram>;

void print_exception(const std::exception& e, int level);
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void ErrorCallback(int error, const char* description);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
//global values, TODO: organize in a struct
float g_ScreenRatio = 1.0f;
entity::LookAtParameters g_look_at_parameters { 0.84f, 0.0f, 30.0f };
entity::RotationAngles g_angles { 0.0f, 0.0f };
controler::CursorState g_cursor { 0, 0, false};
bool g_Paused = false;

typedef struct {
	float width, height;
} WindowSize;
WindowSize g_windowSize {WINDOW_WIDTH,WINDOW_HEIGHT};
entity::PressedKeys g_keys{false, false, false, false};
void game_loop(GLFWwindow *window){

	log("load shaders");
	//carrega os shaders
	auto phong_phong     = load_gpu_program("src/shaders/phong_Blinn-Phong_vertex.glsl","src/shaders/phong_Blinn-Phong_fragment.glsl");
	auto phong_diffuse   = load_gpu_program("src/shaders/phong_diffuse_vertex.glsl","src/shaders/phong_diffuse_fragment.glsl");
	auto gouraud_phong   = load_gpu_program("src/shaders/gouraud_Blinn-Phong_vertex.glsl","src/shaders/gouraud_Blinn-Phong_fragment.glsl");
	auto gouraud_diffuse = load_gpu_program("src/shaders/gouraud_diffuse_vertex.glsl","src/shaders/gouraud_diffuse_fragment.glsl");
	auto wire_renderer   = load_gpu_program("src/shaders/wire_vertex.glsl", "src/shaders/wire_fragment.glsl");
	auto menu_renderer   = load_gpu_program("src/shaders/menu_vertex.glsl", "src/shaders/menu_fragment.glsl");

	log("load meshes");
	
	auto main_menu_mesh = load_mesh("models/main_menu.obj");
	auto new_game_menu_item_mesh = load_mesh("models/new_game_menu_item.obj");

	auto cube_mesh = load_mesh("models/cube.obj");
	auto pawn_mesh = load_mesh("models/pawn.obj");
	auto house_mesh = load_mesh("models/house.obj");
	auto grass_tile_mesh = load_mesh("models/grass_tile.obj");
	auto horizontal_road_tile_mesh = load_mesh("models/horizontal_road_tile.obj");
	auto vertical_road_tile_mesh = load_mesh("models/vertical_road_tile.obj");
	auto cross_section_tile_mesh = load_mesh("models/cross_section_tile.obj");

	log("init enities");

	std::shared_ptr<render::WireMesh> cube_wire_mesh(new render::WireMesh(static_cast<int>(entity::BBoxType::Rectangle)));
	std::shared_ptr<render::WireMesh> cylinder_wire_mesh(new render::WireMesh(static_cast<int>(entity::BBoxType::Cylinder)));

	std::shared_ptr<entity::Player> pawn(new entity::Player(glm::vec4(0,0,-6,1),phong_phong, pawn_mesh));
	pawn->set_wire_mesh(cylinder_wire_mesh);
	pawn->set_wire_renderer(wire_renderer);
	pawn->set_bbox_type(entity::BBoxType::Cylinder);
	pawn->set_bbox_size(2.0f,1.0f,2.0f);  
	pawn->set_speed(0.1f);

	//pawn->set_base_translate(0.0f,3.0f,0.0f);

	std::unique_ptr<controler::Generator> game_generator(
		new controler::Generator(
			phong_phong, phong_diffuse, gouraud_phong, gouraud_diffuse, wire_renderer,
			cube_wire_mesh, cylinder_wire_mesh,
			10,10
		)
	);
	//ading the meshes
	game_generator->insert_mesh(static_cast<int>(controler::MeshIds::ENEMY),pawn_mesh);
	game_generator->insert_mesh(static_cast<int>(controler::MeshIds::POINT),cube_mesh);
	game_generator->insert_mesh(static_cast<int>(controler::MeshIds::CAR),cube_mesh);
	game_generator->insert_mesh(static_cast<int>(controler::MeshIds::HOUSE),house_mesh);

	//' ','+','|','-','#'
	game_generator->insert_tile_mesh(' ', grass_tile_mesh); //grass
	game_generator->insert_tile_mesh('#', grass_tile_mesh); //grass
	game_generator->insert_tile_mesh('+', cross_section_tile_mesh); //asphalt
	game_generator->insert_tile_mesh('|', vertical_road_tile_mesh); //asphalt
	game_generator->insert_tile_mesh('-', horizontal_road_tile_mesh); //asphalt
	game_generator->insert_tile_mesh('C', cross_section_tile_mesh); //asphalt

	log("inicializando o controler");

	controler::GameLoop game_controler(
		std::unique_ptr<entity::Camera>(new entity::Camera(pawn->get_cords())),
		std::unique_ptr<controler::CollisionMap>(new controler::CollisionMap(100,100,10,10)),
		std::move(game_generator),
		pawn,
		phong_phong, phong_diffuse, gouraud_phong, gouraud_diffuse,
		wire_renderer, menu_renderer,
		&g_keys, &g_look_at_parameters,
		&g_angles, &g_cursor,
		&g_ScreenRatio, &g_Paused);

	log("inserindo o inimigo");

	std::shared_ptr<entity::Screen> menu_screen(new entity::Screen(glm::vec4(0.0f,0.0f,0.0f,1.0f), menu_renderer, main_menu_mesh));
	std::shared_ptr<entity::Node> start_node(new entity::Node(glm::vec4(0.0f,0.0f,0.1f,1.0f), menu_renderer, new_game_menu_item_mesh,entity::MenuOptions::Play));
	menu_screen->insert_nodes(start_node,464,362,24,776,800,800);

	game_controler.insert_screen(controler::GameState::MainMenu, menu_screen);

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
			ImGui::Begin("Camera Movement");
            ImGui::SliderFloat("phi", &g_look_at_parameters.phi, 0.0f, 3.14f);
            ImGui::SliderFloat("theta", &g_look_at_parameters.theta, 0.0f, 3.14f);
            ImGui::SliderFloat("distance", &g_look_at_parameters.radius, 2.5f, 40.0f);
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
	srand(time(0));

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsClassic();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
    glEnable(GL_DEPTH_TEST);

	game_loop(window);

    // Finalizamos o uso dos recursos do sistema operacional
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

auto load_gpu_program(const char * vertex, const char * frag) -> std::shared_ptr<render::GPUprogram> {
	std::shared_ptr<render::GPUprogram> renderer;
	try{
		std::shared_ptr<render::GPUprogram> program_expt (new render::GPUprogram(vertex, frag));
		renderer = program_expt;
	}catch(const std::exception& e){
		print_exception(e,0);
		std::exit(EXIT_FAILURE);
	}
	return renderer;
}

auto load_mesh(const char * file) -> std::shared_ptr<render::Mesh>{
	render::ParsedObjMesh load_mesh;
	std::shared_ptr<render::Mesh> res;
	try{
		load_mesh.load_obj_file(file, "models/materials");
		res = load_mesh.load_to_gpu();
	}catch(const std::exception& e){
		print_exception(e,0);
		std::exit(EXIT_FAILURE);
	}
	return res;
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

	if(g_Paused){
		g_angles.angleX = dx/(g_windowSize.width/2)  * 2*PI; // Calcula o ângulo rotação horizontal de acordo com a porcentagem da tela movida (máximo = 2*PI)
    	g_angles.angleY = dy/(g_windowSize.height/2) * 2*PI; // Calcula o ângulo rotação  vertical  de acordo com a porcentagem da tela movida (máximo = 2*PI)
	}
	else{
    	float phi_aux = g_look_at_parameters.phi + 0.01f*dy;
    	if (phi_aux < PHIMAX && phi_aux > PHIMIN)
        	g_look_at_parameters.phi = phi_aux;
		g_look_at_parameters.theta -= 0.01f*dx;
	}
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