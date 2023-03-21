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

uniform sampler2D texture0;

out vec4 color;

void main()
{
    // Obtemos a posição da câmera utilizando a inversa da matriz que define o
    // sistema de coordenadas da câmera.
    vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 camera_position = inverse(view) * origin;

    // O fragmento atual é coberto por um ponto que percente à superfície de um
    // dos objetos virtuais da cena. Este ponto, p, possui uma posição no
    // sistema de coordenadas global (World coordinates). Esta posição é obtida
    // através da interpolação, feita pelo rasterizador, da posição de cada
    // vértice.
    vec4 p = world_pos;

    // Normal do fragmento atual, interpolada pelo rasterizador a partir das
    // normais de cada vértice.
    vec4 n = normalize(normal);

    // Vetor que define o sentido da fonte de luz em relação ao ponto atual.
    vec4 l = normalize(vec4(1.0,1.0,0.0,0.0));

    // Vetor que define o sentido da câmera em relação ao ponto atual.
    vec4 v = normalize(camera_position - p);

    // Vetor que define o sentido da reflexão especular ideal.
    vec4 r = -l + 2*n*dot(n,l);

    // Espectro da fonte de iluminação
    vec3 I  = vec3(1.0, 1.0, 1.0);

    // Espectro da luz ambiente
    vec3 Ia = vec3(0.2, 0.2, 0.2);

    // Equação de Iluminação
    float lambert = max(0,dot(n,l));
    vec3 lambert_diffuse_term = KdIn*I*lambert;             // Termo difuso utilizando a lei dos cossenos de Lambert
    vec3 ambient_term = Ka*Ia;                              // Termo ambiente
    vec3 phong_specular_term = Ks*I*pow(max(0,dot(r,v)),q); // Termo especular utilizando o modelo de iluminação de Phong

    if(using_texture)
    {
        // Obtemos a refletância difusa a partir da leitura da textura
        vec3 Kd = texture(texture0, text_cords).rgb;
        lambert_diffuse_term = Kd * (lambert + 0.01);
    }

    // Cor final do fragmento calculada com uma combinação dos termos difuso, especular, e ambiente.
    //color.rgb = lambert_diffuse_term + ambient_term + phong_specular_term;
	color.rgb = texture(texture0, text_cords).rgb * (lambert + 0.01);
	color.a = 1;

    // Cor final com correção gamma, considerando monitor sRGB.
    color.rgb = pow(color.rgb, vec3(1.0,1.0,1.0)/2.2);
} 

