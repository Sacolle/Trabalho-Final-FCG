#version 330 core

// Atributos de vértice recebidos como entrada ("in") pelo Vertex Shader.
// Veja a função BuildTriangle() em "main.cpp".
layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec2 texture_cords;

uniform mat4 model_transform;  //model values from local cords to global cords
//camera/projection values
uniform mat4 view;
uniform mat4 projection;

out vec2 text_cords;

void main()
{
	vec4 pos = vec4(vertex.xyz,1.0);
	text_cords = texture_cords;
	gl_Position = projection * view * model_transform * pos;
}