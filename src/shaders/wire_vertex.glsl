#version 330 core

// Atributos de vértice recebidos como entrada ("in") pelo Vertex Shader.
// Veja a função BuildTriangle() em "main.cpp".
layout (location = 0) in vec3 vertex;

uniform mat4 transform;  //model values for translation from local cords to global cords and scaleW
//camera/projection values
uniform mat4 view;
uniform mat4 projection;

out vec4 cor_usada;

void main()
{
	gl_Position = projection * view * transform * vec4(vertex.xyz,1.0);

	cor_usada = vec4(0.0,0.0,0.0,1.0);
}