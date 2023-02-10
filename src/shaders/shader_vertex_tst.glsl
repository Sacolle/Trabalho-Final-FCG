#version 330 core

// Atributos de vértice recebidos como entrada ("in") pelo Vertex Shader.
// Veja a função BuildTriangle() em "main.cpp".
layout (location = 0) in vec3 model_coefficients;

uniform mat4 model_scale;
uniform mat4 model_rotate;
uniform mat4 model_translate;
uniform mat4 view;
uniform mat4 projection;
// Atributos de vértice que serão gerados como saída ("out") pelo Vertex Shader.
// ** Estes serão interpolados pelo rasterizador! ** gerando, assim, valores
// para cada fragmento, os quais serão recebidos como entrada pelo Fragment
// Shader. Veja o arquivo "shader_fragment.glsl".
out vec4 cor_interpolada_pelo_rasterizador;

void main()
{
	vec4 temp = vec4(model_coefficients.xyz,1.0);
    gl_Position = projection * view * model_translate * model_rotate * model_scale * temp;

	cor_interpolada_pelo_rasterizador = vec4(1.0,1.0,1.0,1.0);
}