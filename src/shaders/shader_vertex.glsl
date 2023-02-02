#version 330 core

// Atributos de vértice recebidos como entrada ("in") pelo Vertex Shader.
// Veja a função BuildTriangle() em "main.cpp".
layout (location = 0) in vec4 model_coefficients;
layout (location = 1) in vec4 color_coefficients;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
// Atributos de vértice que serão gerados como saída ("out") pelo Vertex Shader.
// ** Estes serão interpolados pelo rasterizador! ** gerando, assim, valores
// para cada fragmento, os quais serão recebidos como entrada pelo Fragment
// Shader. Veja o arquivo "shader_fragment.glsl".
out vec4 cor_interpolada_pelo_rasterizador;

void main()
{
    gl_Position = projection * view * model * model_coefficients;

	cor_interpolada_pelo_rasterizador = color_coefficients;
}

