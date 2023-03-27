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

uniform bool using_texture; //if there is a texture for the model
//values for calculating the lighting
uniform vec3 KdIn;

uniform sampler2D texture0;

out vec4 color_v;

void main()
{
	vec4 pos = vec4(vertex.xyz,1.0);
	vec4 model_pos = pos;
	vec4 world_pos = model_transform * pos;
	vec4 normal = inverse(transpose(model_transform)) * vec4(normals.xyz,0.0);
    normal.w = 0.0;

	// Normal do fragmento atual, interpolada pelo rasterizador a partir das
    // normais de cada vértice.
    vec4 n = normalize(normal);

    // Vetor que define o sentido da fonte de luz em relação ao ponto atual.
    vec4 l = normalize(vec4(1.0,1.0,0.0,0.0));

    // Espectro da fonte de iluminação
    vec3 I  = vec3(1.0, 1.0, 1.0);

    // Equação de Iluminação
    float lambert = max(0,dot(n,l));
	vec3 lambert_diffuse_term;
    if(using_texture)
    {
        // Obtemos a refletância difusa a partir da leitura da textura
        vec3 Kd = texture(texture0, texture_cords).rgb;
        lambert_diffuse_term = Kd * (lambert + 0.01);
    }
    else
    {
		// Termo difuso utilizando a lei dos cossenos de Lambert
        lambert_diffuse_term = KdIn*I*lambert; 
    }

    // Cor final calculada através do termo difuso
    color_v.rgb = lambert_diffuse_term;

	gl_Position = projection * view * model_transform * pos;
}