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
    vec4  flashlight_pos = player_pos - vec4(0.0,1.0,0.0,0.0);
    vec4  camera_dir = player_pos - camera_position;
    camera_dir.y = 0;
    vec4  flashlight_dir   = normalize(camera_dir);
    float flashlight_range = 32.0;
    float flashlight_angle = radians(15.0);
    vec4  light_pos   = player_pos + vec4(0.0,10.0,0.0,0.0);
    vec4  light_dir   = normalize(vec4(0.0,-1.0,0.0,0.0));
    float light_angle = radians(30.0);
    vec4  l = normalize(light_pos - p);

    // Vetor que define o sentido da câmera em relação ao ponto atual.
    vec4 v = normalize(camera_position - p);

    // Vetor que define o sentido da reflexão especular ideal.
    vec4 r = -l + 2*n*dot(n,l);

    // Espectro da fonte de iluminação
    vec3 I  = vec3(0.7, 0.7, 0.7);

    // Espectro da luz ambiente
    vec3 Ia = vec3(0.0000001, 0.0000001, 0.0000001);

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
    bool  iluminado_spotlight  = true;
    bool  iluminado_flashlight = true;
    float spotlight_on_pixel   = dot(normalize(p - light_pos),light_dir);
    float flashlight_on_pixel  = dot(normalize(p - flashlight_pos),flashlight_dir);
    float spotlight_cut_off    = cos(light_angle);
    float flashlight_cut_off   = cos(flashlight_angle);
    float flashlight_distance  = length(p - flashlight_pos);
    if(spotlight_on_pixel < spotlight_cut_off)
        iluminado_spotlight  = false;
    if(flashlight_on_pixel < flashlight_cut_off || flashlight_distance > flashlight_range)
        iluminado_flashlight = false;
    if(!iluminado_spotlight && !iluminado_flashlight)
        color.rgb = ambient_term;
    else{
        float intensity_flashlight = 0.0, intensity_spotlight = 0.0;
        if(iluminado_flashlight){
            intensity_flashlight = 1.0 - (1 - flashlight_on_pixel) / (1 - flashlight_cut_off);
            intensity_flashlight = min(intensity_flashlight, 1.0 - (1 - flashlight_distance) / (1 - flashlight_range));
        }
            
        if(iluminado_spotlight)
            intensity_spotlight  = 1.0 - (1 - spotlight_on_pixel) / (1 - spotlight_cut_off);
        float intensity = max(intensity_flashlight, intensity_spotlight);
        color.rgb = (lambert_diffuse_term + ambient_term + phong_specular_term) * intensity;
    }

    // Cor final com correção gamma, considerando monitor sRGB.
    //color.rgb = pow(color.rgb, vec3(1.0,1.0,1.0)/2.2);
} 

