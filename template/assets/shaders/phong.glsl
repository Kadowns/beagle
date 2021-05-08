#if !PHONG_GLSL
#define PHONG_GLSL 1

#include "light.glsl"

vec3 calculate_directional_light_phong(in DirectionalLight light, in vec3 position, in vec3 normal, in vec3 viewDirection){
    vec3 lightDirection = normalize(light.direction);

    float diff = max(dot(normal, lightDirection), 0.0);
    vec3 diffuse = diff * light.color.xyz * light.color.w;

    vec3 reflectDir = reflect(-lightDirection, normal);
    float spec = pow(max(dot(viewDirection, reflectDir), 0.0), 32);
    vec3 specular = 0.5f * spec * light.color.xyz;

    return diffuse + specular;
}

vec3 calculate_point_light_phong(in PointLight light, in vec3 position, in vec3 normal, in vec3 viewDirection) {
    float distance = length(light.position - position);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    vec3 lightDirection = (position - light.position) / distance;

    float diff = max(dot(normal, lightDirection), 0.0);
    vec3 diffuse = diff * light.color.xyz * light.color.w * attenuation;

    vec3 reflectDir = reflect(-lightDirection, normal);
    float spec = pow(max(dot(viewDirection, reflectDir), 0.0), 32);
    vec3 specular = 0.5f * spec * light.color.xyz * attenuation;

    return diffuse + specular;
}

vec3 calculate_illumination_phong(in Illumination illumination, in vec3 position, in vec3 normal, in vec3 viewPosition){
    vec3 result = vec3(0);
    vec3 viewDirection = normalize(viewPosition - position);
    for (uint i = 0; i < illumination.directionalLightCount; i++){
        result += calculate_directional_light_phong(illumination.directionalLights[i], position, normal, viewDirection);
    }
    for (uint i = 0; i < illumination.pointLightCount; i++){
        result += calculate_point_light_phong(illumination.pointLights[i], position, normal, viewDirection);
    }
    return result;
}

#endif