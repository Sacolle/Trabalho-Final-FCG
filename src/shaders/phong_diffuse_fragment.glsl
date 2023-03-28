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
uniform vec4 player_pos;

uniform sampler2D texture0;

out vec4 color;

void main()
{
    // Normal do fragmento atual, interpolada pelo rasterizador a partir das
    // normais de cada vértice.
    vec4 n = normalize(normal);

    // Vetor que define o sentido da fonte de luz em relação ao ponto atual.
    vec4 light_pos = player_pos + vec4(0.0,5.0,0.0,0.0);
    vec4 l = normalize(light_pos - world_pos);

    // Espectro da fonte de iluminação
    vec3 I  = vec3(1.0, 1.0, 1.0);

    // Equação de Iluminação
    float lambert = max(0,dot(n,l));
    vec3 lambert_diffuse_term;

    if(using_texture)
    {
        // Obtemos a refletância difusa a partir da leitura da textura
        vec3 Kd = texture(texture0, text_cords).rgb;
        lambert_diffuse_term = Kd * (lambert + 0.01);
    }
    else
    {
        lambert_diffuse_term = KdIn*I*lambert;             // Termo difuso utilizando a lei dos cossenos de Lambert
    }

    // Cor final do fragmento calculada com uma combinação dos termos difuso, especular, e ambiente.
    color.rgb = lambert_diffuse_term;

    // Cor final com correção gamma, considerando monitor sRGB.
    //color.rgb = pow(color.rgb, vec3(1.0,1.0,1.0)/2.2);
} 

