#include <cmath>
#include <cstdio>
#include <cstdlib>
#define _USE_MATH_DEFINES

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
#include "shader.hpp"
#include "matrix.hpp"
#include "entities.hpp"
#include "mesh.hpp"
#include "collision.hpp"


#define PI 3.141592f

void print_exception(const std::exception& e, int level);
auto player_movement() -> float;
auto camera_movement() -> Direction;

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
bool freeCam = false;
float angleX = 0.0f;
float angleZ = 0.0f;

typedef struct {
	float width, height;
} WindowSize;
WindowSize windowSize;

typedef struct {
	bool w, a, s, d;
} PressedKeys;
PressedKeys g_keys{false, false, false, false};

void game_loop(GLFWwindow *window, const char *vertex_shader, const char *fragment_shader){
	//carrega os shaders
	std::shared_ptr<render::GPUprogram> gpu_program;
	try{
		std::shared_ptr<render::GPUprogram> gpu_program_expt (new render::GPUprogram(vertex_shader,fragment_shader));
		gpu_program = gpu_program_expt;
	}catch(const std::exception& e){
		print_exception(e,0);
		std::exit(EXIT_FAILURE);
	}
	std::unique_ptr<entity::Camera> camera (new entity::Camera(true));

	std::shared_ptr<render::ObjMesh> teapot_mesh;
	std::shared_ptr<render::ObjMesh> plane_mesh;
	std::shared_ptr<render::ObjMesh> cube_mesh;
	try{
		std::shared_ptr<render::ObjMesh> teapot_expt (new render::ObjMesh("models/teapot.obj", "models/materials"));
		std::shared_ptr<render::ObjMesh> plane_expt (new render::ObjMesh("models/plane.obj", "models/materials"));
		std::shared_ptr<render::ObjMesh> cube_expt (new render::ObjMesh("models/cube.obj", "models/materials"));
		teapot_mesh = teapot_expt;
		plane_mesh = plane_expt;
		cube_mesh = cube_expt;
	}catch(const std::exception& e){
		print_exception(e,0);
		std::exit(EXIT_FAILURE);
	}

	std::shared_ptr<render::WireMesh> cube_wire_mesh(new render::WireMesh(static_cast<int>(entity::BBoxType::Rectangle)));
	std::shared_ptr<render::WireMesh> cylinder_wire_mesh(new render::WireMesh(static_cast<int>(entity::BBoxType::Cylinder)));

	std::shared_ptr<entity::Entity> plane(
		new entity::Entity(
			gpu_program, plane_mesh, nullptr,
			glm::vec4(0,0,0,1), glm::vec4(0,1,0,0), 0.01f,
			0, 0, 0,
			4, 1, 4,
			1, 1, 0, entity::BBoxType::Rectangle
		)
	);
  
	std::shared_ptr<entity::Entity> player(
		new entity::Entity(
			gpu_program, teapot_mesh, cylinder_wire_mesh,
			glm::vec4(4,0,4,1), glm::vec4(1,0,0,0), 0.05f,
			0, 0, 0,
			1, 1, 1,
			2, 2, 1.25, entity::BBoxType::Cylinder
		)
	);
	std::shared_ptr<entity::Entity> cube(
		new entity::Entity(
			gpu_program, cube_mesh, cube_wire_mesh,
			glm::vec4(0.01,0,0.01,1), glm::vec4(1,0,0,0), 0.01f,
			0, 0, 0,
			1, 1, 1,
			1, 1, 1, entity::BBoxType::Rectangle
		)
	);

	utils::CollisionMap collision_map(100,100,2,2);
	collision_map.insert_mover(player);
	collision_map.insert_mover(cube);
	
	float phi = 0.0f, theta = 0.0f, distance = 8.0f;
	float x = 0, y = -1.25, z = 0;
	bool show_camera_movment = true;
    // Ficamos em um loop infinito, renderizando, até que o usuário feche a janela
    while (!glfwWindowShouldClose(window))
    {
		glfwPollEvents();
  
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		{
			ImGui::Begin("Editor");                          // Create a window called "Hello, world!" and append into it.
			ImGui::Checkbox("Camera Movment",&show_camera_movment);
            ImGui::Text("Base translate");               // Display some text (you can use a format strings too)
            ImGui::SliderFloat("x", &x, -5.0f, 5.0f);
            ImGui::SliderFloat("y", &y, -5.0f, 5.0f);
            ImGui::SliderFloat("z", &z, -5.0f, 5.0f);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
		}
		if(show_camera_movment){
			ImGui::Begin("Camera Movment");
            ImGui::Text("Move the camera");               // Display some text (you can use a format strings too)
            ImGui::SliderFloat("phi", &phi, 0.0f, 3.14f);
            ImGui::SliderFloat("theta", &theta, 0.0f, 3.14f);
            ImGui::SliderFloat("distance", &distance, 2.5f, 20.0f);

			ImGui::End();
		}

		ImGui::Render();
  
        glClearColor(1.0f, 0.2f, 0.2f, 1.0f); // define a cor de fundo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // pinta os pixels do framebuffer 

		//set GPU and camera projections
		gpu_program->use_prog();

		if(!freeCam){
			camera->update_position(phi,theta,distance);

			const float angle = player_movement();
			if(angle != -1){
				player->set_y_angle(angle);
				collision_map.remove_mover(player);
				if(!collision_map.colide_foward(player)){
					player->translate_foward();
				}
				collision_map.insert_mover(player);
			}
		}
		else{
			Direction dir = camera_movement();
			camera->update_position(dir);
			camera->update_view(&angleX, &angleZ);
		}
		
		camera->update_aspect_ratio(g_ScreenRatio);
		gpu_program->set_uniform_mtx("view",camera->get_view_ptr());
		gpu_program->set_uniform_mtx("projection",camera->get_projection_ptr());

		//correction translation to set the origin at the center of the model
		player->set_base_translate(x,y,z);
		
		player->draw();
		player->draw_wire();

		cube->draw();
		cube->draw_wire();

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
}
auto player_movement() -> float{
	/*
	TODO: setup an interpolation of positions, so as the rotations to the foward direction feel smoother
	fowards is -z
	right is x
	*/
	float components = 0;
	float angle = 0;
	if(g_keys.w){
		angle += PI/2;
		components++;
	}
	if(g_keys.a){
		angle += PI;
		components++;
	}
	if(g_keys.s){
		angle += PI + PI/2;
		components++;
	}
	if(g_keys.d){
		if(angle >= PI){
			angle += PI + PI;
		}else{
			angle += 0;
		}
		components++;
	}
	if(components == 0) return -1;
	return angle/components;
}
auto camera_movement() -> Direction{
	if(g_keys.w){
		return Front;
	}
	if(g_keys.a){
		return Left;
	}
	if(g_keys.s){
		return Back;
	}
	if(g_keys.d){
		return Right;
	}
	return Stay;
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
    GLFWwindow* window = glfwCreateWindow(800, 800, "INF01047 - 00333916 - Pedro Henrique Boniatti Colle / 00334087 - Eduardo Dalmás Faé", NULL, NULL);
    if (!window){
        glfwTerminate();
       	std::cerr << "ERROR: glfwCreateWindow() failed.\n" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    //callback para o resize da janela, em q se deve realocar a memoria
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    glfwSetWindowSize(window, 800, 800); // Forçamos a chamada do callback acima, para definir g_ScreenRatio.
	windowSize.width  = 800;
	windowSize.height = 800;

    glfwSetMouseButtonCallback(window, MouseButtonCallback); //botao do mouse
    glfwSetCursorPosCallback(window, CursorPosCallback); //movimentar o cursor
    glfwSetKeyCallback(window, KeyCallback); //keypress

	glfwMakeContextCurrent(window); //faz com q as chamadas do OpenGL façam render na janela
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress); //load das funcões da Glad

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
	float dx = xpos - g_LastCursorPosX;
    float dy = ypos - g_LastCursorPosY;

	// Atualizamos as variáveis globais para armazenar a posição atual do
    // cursor como sendo a última posição conhecida do cursor.
    g_LastCursorPosX = xpos;
    g_LastCursorPosY = ypos;

	if(!freeCam) return;

    angleX = dx/(windowSize.width/2)  * 2*PI; // Calcula o ângulo rotação horizontal de acordo com a porcentagem da tela movida (máximo = 2*PI)
    angleZ = dy/(windowSize.height/2) * 2*PI; // Calcula o ângulo rotação  vertical  de acordo com a porcentagem da tela movida (máximo = 2*PI)
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
		freeCam = !freeCam;
		if(freeCam)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		else
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
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

	windowSize.width  = width;   // Salva a largura da tela
    windowSize.height = height; // Salva a altura da tela
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