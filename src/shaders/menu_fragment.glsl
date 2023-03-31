#version 330 core

in vec2 text_cords;

uniform bool using_texture; //if there is a texture for the model
//values for calculating the lighting
uniform vec3 Ka;
uniform vec3 KdIn;
uniform vec3 Ks;  
uniform float q;

uniform sampler2D texture0;

out vec4 color;

void main()
{
    if(using_texture)
    {
		color.rgb = texture(texture0, text_cords).rgb;
    }else{
		color.rgb = vec3(0.0,0.0,1.0);
	}
	color.a = 1;
} 

