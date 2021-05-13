#if !PBR_MATERIAL_GLSL
#define PBR_MATERIAL_GLSL 1

struct PBRMaterial {
    vec4  albedo;
    float metallic;
    float roughness;
    float ao;
};

#endif