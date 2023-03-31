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

out vec4 color_v;

vec4 getFlashlightDir(vec4 camera_position){
    if(paused) return normalize(camera_dir);

    vec4 flashlight_dir = player_pos - camera_position;
    flashlight_dir.y = 0;
    return normalize(flashlight_dir);
}

vec4 getFlashlightPos(vec4 camera_position, vec4 flashlight_dir){
    if(paused)
        return camera_position;
    else
        return player_pos + vec4(0.0,2.0,0.0,0.0) - flashlight_dir * 8;
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

    // Espectro da fonte de iluminação
    vec3 I  = vec3(0.7, 0.7, 0.7);

    // Espectro da luz ambiente
    vec3 Ia = vec3(0.04, 0.04, 0.1);
    
    // Informações da flashlight
    vec4  flashlight_dir   = getFlashlightDir(camera_position);
    vec4  flashlight_pos   = getFlashlightPos(camera_position, flashlight_dir);
    float flashlight_angle = radians(15.0);
    float flashlight_range = 50.0;

    // Informações da spotlight
    vec4  spotlight_pos    = player_pos + vec4(0.0,10.0,0.0,0.0);
    vec4  spotlight_dir    = vec4(0.0,-1.0,0.0,0.0);
    float spotlight_angle  = radians(30.0);

    // Testa se o pixel é mais afetado pela spotlight ou flashlight e calcula a intensidade
    float intensity = getIntensity(world_pos, flashlight_dir, flashlight_pos, flashlight_angle, flashlight_range, spotlight_pos, spotlight_dir, spotlight_angle);

    // Vetor que define o sentido da fonte de luz em relação ao ponto atual.
    vec4 l;
    if(paused)
        l = normalize(flashlight_pos - world_pos);
    else
        l = normalize(spotlight_pos - world_pos);

    // Equação de Iluminação
    float lambert = max(0,dot(n,l));
    vec3  lambert_diffuse_term = KdIn*I*lambert; // Termo difuso utilizando a lei dos cossenos de Lambert
    vec3  ambient_term = Ka*Ia;                  // Termo ambiente

   // Obtemos a refletância difusa a partir da leitura da textura
    if(using_texture)
        lambert_diffuse_term = texture(texture0, texture_cords).rgb * I * lambert;

    // Calcula o valor da luz ambiente
    vec3 ambient_light = ambient_term + lambert_diffuse_term * 0.02;

    // Cor final do fragmento calculada com uma combinação dos termos difuso, especular, e ambiente.
    if(intensity == 0.0)
        color_v.rgb = ambient_light;
    else
        color_v.rgb = lambert_diffuse_term * intensity + ambient_light;

	gl_Position = projection * view * model_transform * pos;
}