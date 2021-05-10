#if !LIGHT_GLSL
#define LIGHT_GLSL 1

struct DirectionalLight {
    vec4 color;
    vec3 direction;
};

struct PointLight {
    vec4 color;
    vec3 position;
    float constant;
    float linear;
    float quadratic;
};

struct Illumination {
    DirectionalLight directionalLights[4];
    uint directionalLightCount;
    PointLight pointLights[16];
    uint pointLightCount;
};

#endif