#version 330 core

in vec4 world_pos;
in vec4 model_pos;
in vec4 normal;
in vec2 text_cords;

uniform mat4 model_transform;  //model values from local cords to global cords
//camera/projection values
uniform mat4 view;
uniform mat4 projection;

uniform bool using_texture; //if there is a texture for the model
//values for calculating the lighting
uniform vec3 Ka;
uniform vec3 KdIn;
uniform vec3 Ks;  
uniform float q;

uniform sampler2D texture;

out vec4 color;

void main()
{
    // Definimos a cor final de cada fragmento utilizando a cor interpolada
    // pelo rasterizador.
    color = vec4(1.0,1.0,1.0,1.0);
} 

