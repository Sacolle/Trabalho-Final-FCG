#version 330 core

// Atributos de vértice recebidos como entrada ("in") pelo Vertex Shader.
// Veja a função BuildTriangle() em "main.cpp".
layout (location = 0) in vec3 model_coefficients;

//model values
uniform mat4 model_scale;
uniform mat4 model_rotate;
uniform mat4 model_translate;

//only when rendering the model
//its a correctional translation to center (0,0,0) at the models center
uniform mat4 model_base_translate;

//wire mesh values
uniform bool render_wire_frame;
uniform mat4 bbox_scale;

//camera/projection values
uniform mat4 view;
uniform mat4 projection;
// Atributos de vértice que serão gerados como saída ("out") pelo Vertex Shader.
// ** Estes serão interpolados pelo rasterizador! ** gerando, assim, valores
// para cada fragmento, os quais serão recebidos como entrada pelo Fragment
// Shader. Veja o arquivo "shader_fragment.glsl".
out vec4 cor_interpolada_pelo_rasterizador;

void main()
{
	if(render_wire_frame){
		vec4 temp = vec4(model_coefficients.xyz,1.0);
		gl_Position = projection * view * model_translate * model_rotate * model_scale *  bbox_scale * temp;

		cor_interpolada_pelo_rasterizador = vec4(0.0,0.0,0.0,1.0);
	}else{
		vec4 temp = vec4(model_coefficients.xyz,1.0);
		gl_Position = projection * view * model_translate * model_rotate * model_scale * model_base_translate * temp;

		cor_interpolada_pelo_rasterizador = vec4(1.0,1.0,1.0,1.0);
	}
}