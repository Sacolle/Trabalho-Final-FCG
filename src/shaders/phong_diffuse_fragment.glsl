#version 330 core

in vec4 world_pos;
in vec4 model_pos;
in vec4 normal;
in vec2 text_cords;

uniform mat4 model_transform;  //model values from local cords to global cords
//camera/projection values
uniform mat4 view;
uniform mat4 projection;


//values for calculating the lighting
uniform vec3 Ka;
uniform vec3 KdIn;

//if there is a texture for the model
uniform bool using_texture; 
uniform sampler2D texture0;

//values for calculating the light position
uniform vec4 player_pos;
uniform bool paused;
uniform vec4 camera_dir;

out vec4 color;
#define FLASHLIGHTOFFSET 8.0

vec4 getFlashlightDir(vec4 camera_position){
    if(paused)
		return normalize(camera_dir);

    vec4 flashlight_dir = player_pos - camera_position;
    flashlight_dir.y = 0;
    return normalize(flashlight_dir);
}

vec4 getFlashlightPos(vec4 camera_position, vec4 flashlight_dir){
    if(paused)
        return camera_position;
	else
        return player_pos + vec4(0.0,2.0,0.0,0.0) - flashlight_dir * FLASHLIGHTOFFSET;
}

float getIntensity(vec4 p, vec4 flashlight_dir, vec4 flashlight_pos, float flashlight_angle, float flashlight_range, vec4 spotlight_pos, vec4 spotlight_dir, float spotlight_angle){
    float intensity;
    bool  iluminado_spotlight  = true;
    bool  iluminado_flashlight = true;
    float spotlight_on_pixel   = dot(normalize(p - spotlight_pos),spotlight_dir);
    float flashlight_on_pixel  = dot(normalize(p - flashlight_pos),flashlight_dir);
    float spotlight_cut_off    = cos(spotlight_angle);
    float flashlight_cut_off   = cos(flashlight_angle);
    float flashlight_distance  = length(p - flashlight_pos);

    if(spotlight_on_pixel < spotlight_cut_off)
        iluminado_spotlight  = false;
    if(flashlight_on_pixel < flashlight_cut_off || flashlight_distance > flashlight_range)
        iluminado_flashlight = false;

    if(!iluminado_spotlight && !iluminado_flashlight){
        intensity = 0.0;
    }
    else{
        float intensity_flashlight = 0.0;
        float intensity_spotlight  = 0.0;

        if(iluminado_flashlight){
            intensity_flashlight = 1.0 - (1.0 - flashlight_on_pixel) / (1.0 - flashlight_cut_off);
            intensity_flashlight = min(intensity_flashlight, 1.0 - (1.0 - flashlight_distance) / (1.0 - flashlight_range));
        }

        if(iluminado_spotlight)
            intensity_spotlight  = 1.0 - (1.0 - spotlight_on_pixel) / (1.0 - spotlight_cut_off);
        
        intensity = max(intensity_flashlight, intensity_spotlight);
    }

    return intensity;
}

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

    // Espectro da fonte de iluminação
    vec3 I  = vec3(0.7, 0.7, 0.7);

    // Espectro da luz ambiente
    vec3 Ia = vec3(0.04, 0.04, 0.1);
    
    // Informações da flashlight
    vec4  flashlight_dir   = getFlashlightDir(camera_position);
    vec4  flashlight_pos   = getFlashlightPos(camera_position, flashlight_dir);
    float flashlight_angle = radians(15.0);
    float flashlight_range = 50.0 + FLASHLIGHTOFFSET;

    // Informações da spotlight
    vec4  spotlight_pos    = player_pos + vec4(0.0,10.0,0.0,0.0);
    vec4  spotlight_dir    = vec4(0.0,-1.0,0.0,0.0);
    float spotlight_angle  = radians(30.0);

    // Testa se o pixel é mais afetado pela spotlight ou flashlight e calcula a intensidade
    float intensity = getIntensity(p, flashlight_dir, flashlight_pos, flashlight_angle, flashlight_range, spotlight_pos, spotlight_dir, spotlight_angle);

    // Vetor que define o sentido da fonte de luz em relação ao ponto atual.
    vec4 l;
    if(paused)
        l = normalize(flashlight_pos - p);
    else
        l = normalize(spotlight_pos - p);

    // Equação de Iluminação
    float lambert = max(0,dot(n,l));
    vec3  lambert_diffuse_term = KdIn*I*lambert; // Termo difuso utilizando a lei dos cossenos de Lambert
    vec3  ambient_term = Ka*Ia;                  // Termo ambiente

    // Obtemos a refletância difusa a partir da leitura da textura
    if(using_texture)
        lambert_diffuse_term = texture(texture0, text_cords).rgb * I * lambert;

    // Calcula o valor da luz ambiente
    vec3 ambient_light = ambient_term + lambert_diffuse_term * 0.02;

    // Cor final do fragmento calculada com uma combinação dos termos difuso, especular, e ambiente.
    if(intensity == 0.0)
        color.rgb = ambient_light;
    else
        color.rgb = lambert_diffuse_term * intensity + ambient_light;
}

