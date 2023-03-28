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
uniform vec3 Ka;
uniform vec3 KdIn;
uniform vec3 Ks;  
uniform float q;
uniform vec4 player_pos;

uniform sampler2D texture0;

out vec4 color_v;

void main()
{
	vec4 pos = vec4(vertex.xyz,1.0);
	vec4 model_pos = pos;
	vec4 world_pos = model_transform * pos;
	vec4 normal = inverse(transpose(model_transform)) * vec4(normals.xyz,0.0);
    normal.w = 0.0;

	// Obtemos a posição da câmera utilizando a inversa da matriz que define o
    // sistema de coordenadas da câmera.
    vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 camera_position = inverse(view) * origin;

    // Normal do fragmento atual, interpolada pelo rasterizador a partir das
    // normais de cada vértice.
    vec4 n = normalize(normal);

    // Vetor que define o sentido da fonte de luz em relação ao ponto atual.
    vec4  light_pos   = player_pos + vec4(0.0,30.0,0.0,0.0);
    vec4  light_dir   = normalize(vec4(0.0,-1.0,0.0,0.0));
    float light_angle = radians(30.0);
    vec4  l = normalize(light_pos - world_pos);


    // Vetor que define o sentido da câmera em relação ao ponto atual.
    vec4 v = normalize(camera_position - world_pos);

    // Vetor que define o sentido da reflexão especular ideal.
    vec4 r = -l + 2*n*dot(n,l);

    // Espectro da fonte de iluminação
    vec3 I  = vec3(0.7, 0.7, 0.7);

    // Espectro da luz ambiente
    vec3 Ia = vec3(0.3, 0.1, 0.1);

    // Equação de Iluminação
    float lambert = max(0,dot(n,l));
    vec3 lambert_diffuse_term = KdIn*I*lambert;             // Termo difuso utilizando a lei dos cossenos de Lambert
    vec3 ambient_term = Ka*Ia;                              // Termo ambiente
    vec3 phong_specular_term = Ks*I*pow(max(0,dot(r,v)),q); // Termo especular utilizando o modelo de iluminação de Phong

    if(using_texture)
    {
        // Obtemos a refletância difusa a partir da leitura da textura
        vec3 Kd = texture(texture0, texture_cords).rgb;
        lambert_diffuse_term = Kd * (lambert + 0.01);
    }

    // Cor final do fragmento calculada com uma combinação dos termos difuso, especular, e ambiente.
    if(dot(normalize(world_pos - light_pos),light_dir) < cos(light_angle))
        color_v.rgb = ambient_term;
    else
        color_v.rgb = lambert_diffuse_term + ambient_term + phong_specular_term;

	gl_Position = projection * view * model_transform * pos;
}