#if !PHONG_GLSL
#define PHONG_GLSL 1

#include "../light.glsl"
#include "phong_material.glsl"

vec3 calculate_directional_light_phong(in DirectionalLight light, in vec3 position, in vec3 normal, in vec3 view, in PhongMaterial material){
    vec3 lightDirection = normalize(light.direction);

    float diff = max(dot(normal, lightDirection), 0.0);
    vec3 diffuse = (material.diffuse * diff) * light.color.xyz * light.color.w;

    vec3 reflectDir = reflect(-lightDirection, normal);
    float spec = pow(max(dot(view, reflectDir), 0.0), material.specular.w);
    vec3 specular = (spec * material.specular.xyz) * light.color.xyz;

    return diffuse + specular;
}

vec3 calculate_point_light_phong(in PointLight light, in vec3 position, in vec3 normal, in vec3 view, in PhongMaterial material) {
    float distance = length(light.position - position);
    float attenuation = 1.0 / (distance * distance);
    vec3 lightDirection = (light.position - position) / distance;

    float diff = max(dot(normal, lightDirection), 0.0);
    vec3 diffuse = (material.diffuse * diff) * light.color.xyz * attenuation;

    vec3 reflectDir = reflect(-lightDirection, normal);
    float spec = pow(max(dot(view, reflectDir), 0.0), material.specular.w);
    vec3 specular = (material.specular.xyz * spec) * light.color.xyz * attenuation;

    return diffuse + specular;
}

vec3 calculate_illumination_phong(in Illumination illumination, in vec3 position, in vec3 normal, in vec3 viewPosition, in PhongMaterial material){
    vec3 result = vec3(0);
    vec3 view = normalize(viewPosition - position);
    for (uint i = 0; i < illumination.directionalLightCount; i++){
        result += calculate_directional_light_phong(illumination.directionalLights[i], position, normal, view, material);
    }
    for (uint i = 0; i < illumination.pointLightCount; i++){
        result += calculate_point_light_phong(illumination.pointLights[i], position, normal, view, material);
    }
    return result;
}

#endif