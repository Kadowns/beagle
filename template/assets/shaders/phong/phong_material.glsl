#if !PHONG_MATERIAL_GLSL
#define PHONG_MATERIAL_GLSL 1

struct PhongMaterial {
    vec3 diffuse;
    vec4 specular;
};

#endif